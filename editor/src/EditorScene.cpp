#include <QBlock/EditorScene.h>
#include <QBlock/ConnectionWidget.h>
#include <QBlock/NodePickerDialog.h>
#include <QBlock/PortWidget.h>
#include <QBlock/Translator.h>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QApplication>

namespace QBlock {

EditorScene::EditorScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(-2000, -2000, 4000, 4000);
    setBackgroundBrush(ThemeManager::background());

    // One-time warmup: force QImage/QPixmap paint engine init before first paint
    static bool warmupDone = false;
    if (!warmupDone) {
        warmupDone = true;
        QPixmap pm(1, 1);
        QImage img(1, 1, QImage::Format_ARGB32);
        QPainter p(&pm);
        p.fillRect(0, 0, 1, 1, Qt::black);
        p.end();
        QPainter p2(&img);
        p2.fillRect(0, 0, 1, 1, Qt::black);
        p2.end();
    }

    // Refresh background when theme changes
    ThemeManager::onThemeChanged([this]() {
        setBackgroundBrush(ThemeManager::background());
        update();
    });
}

EditorScene::~EditorScene() = default;

void EditorScene::setGraph(NodeGraph* graph) {
    clear();
    graph_ = graph;
    if (graph_) {
        rebuildFromGraph();
    }
}

void EditorScene::rebuildFromGraph() {
    if (!graph_) return;

    nodeWidgetMap_.clear();
    QGraphicsScene::clear();

    for (const auto& node : graph_->nodes()) {
        addNodeWidget(node.get());
    }

    for (const auto& conn : graph_->connections()) {
        auto* srcNode = conn->source()->parentNode();
        auto* tgtNode = conn->target()->parentNode();
        auto* srcWidget = findNodeWidget(srcNode);
        auto* tgtWidget = findNodeWidget(tgtNode);
        if (srcWidget && tgtWidget) {
            auto* srcPort = srcWidget->findPortWidget(conn->source()->name(), false);
            auto* tgtPort = tgtWidget->findPortWidget(conn->target()->name(), true);
            if (srcPort && tgtPort) {
                addConnectionWidget(srcPort, tgtPort);
            }
        }
    }
}

void EditorScene::syncPositionsToGraph() {
    if (!graph_) return;
    for (auto& [node, widget] : nodeWidgetMap_) {
        node->setPosition(static_cast<float>(widget->pos().x()),
                          static_cast<float>(widget->pos().y()));
    }
}

NodeWidget* EditorScene::addNodeWidget(Node* node) {
    auto* widget = new NodeWidget(node);
    addItem(widget);
    nodeWidgetMap_[node] = widget;
    return widget;
}

void EditorScene::removeNodeWidget(Node* node) {
    auto it = nodeWidgetMap_.find(node);
    if (it != nodeWidgetMap_.end()) {
        removeItem(it->second);
        delete it->second;
        nodeWidgetMap_.erase(it);
    }
}

NodeWidget* EditorScene::findNodeWidget(Node* node) const {
    auto it = nodeWidgetMap_.find(node);
    return it != nodeWidgetMap_.end() ? it->second : nullptr;
}

ConnectionWidget* EditorScene::addConnectionWidget(PortWidget* source, PortWidget* target) {
    if (!source || !target) return nullptr;
    if (source->port()->direction() != PortDirection::Output) return nullptr;
    if (target->port()->direction() != PortDirection::Input) return nullptr;

    if (!typesCompatible(source->port()->type(), target->port()->type()))
        return nullptr;

    Connection* conn = nullptr;
    if (graph_) {
        conn = graph_->connect(source->port(), target->port());
        if (!conn) return nullptr;
    }

    auto* widget = new ConnectionWidget(source, target, conn);
    addItem(widget);
    return widget;
}

void EditorScene::removeConnectionWidget(ConnectionWidget* widget) {
    if (!widget) return;

    if (graph_ && widget->connection()) {
        graph_->disconnect(widget->connection());
    }

    if (widget->sourcePort())
        widget->sourcePort()->removeConnection(widget);
    if (widget->targetPort())
        widget->targetPort()->removeConnection(widget);

    removeItem(widget);
    delete widget;
}

void EditorScene::clear() {
    nodeWidgetMap_.clear();
    QGraphicsScene::clear();
}

// ---- Port drag handling ----

void EditorScene::startPortDrag(PortWidget* source) {
    if (!source || source->port()->direction() != PortDirection::Output) return;
    dragSourcePort_ = source;
    tempConnection_ = new TempConnectionWidget(dragSourcePort_);
    addItem(tempConnection_);
}

void EditorScene::updatePortDrag(const QPointF& scenePos) {
    if (tempConnection_) {
        tempConnection_->updateEndPoint(scenePos);
    }
}

void EditorScene::endPortDrag(const QPointF& scenePos) {
    if (!tempConnection_ || !dragSourcePort_) {
        cleanupTempConnection();
        return;
    }

    auto* item = itemAt(scenePos, QTransform());
    auto* targetPort = dynamic_cast<PortWidget*>(item);

    bool connected = false;
    if (targetPort) {
        if (targetPort->port()->direction() == PortDirection::Input &&
            targetPort->port()->parentNode() != dragSourcePort_->port()->parentNode()) {
            if (typesCompatible(dragSourcePort_->port()->type(),
                                targetPort->port()->type())) {
                auto* cw = addConnectionWidget(dragSourcePort_, targetPort);
                if (cw) {
                    connected = true;
                    emit connectionCreated(cw->connection());
                    emit graphModified();
                }
            }
        }
    }

    cleanupTempConnection();

    if (connected) {
        QGraphicsSceneMouseEvent dummy;
        dummy.accept();
    }
}

void EditorScene::showNodePicker(const QPointF& scenePos) {
    if (!dragSourcePort_ || !typeLister_ || !nodeCreator_) return;

    DataType srcType = dragSourcePort_->port()->type();
    bool isInput = true; // We need nodes that accept this type

    NodePickerDialog dialog(srcType, isInput, typeLister_, QApplication::activeWindow());
    if (dialog.exec() == QDialog::Accepted) {
        std::string selectedType = dialog.selectedType();
        if (!selectedType.empty()) {
            // Create the node at the drop position
            float nx = static_cast<float>(scenePos.x());
            float ny = static_cast<float>(scenePos.y());
            Node* newNode = nodeCreator_(selectedType, nx, ny);

            if (newNode) {
                // Try to auto-connect from the dragged port to the new node's first compatible input
                DataType srcDataType = dragSourcePort_->port()->type();
                for (const auto& input : newNode->inputs()) {
                    if (typesCompatible(srcDataType, input->type())) {
                        auto* srcWidget = dragSourcePort_;
                        auto* newNodeWidget = findNodeWidget(newNode);
                        if (newNodeWidget) {
                            auto* tgtPort = newNodeWidget->findPortWidget(input->name(), true);
                            if (tgtPort) {
                                auto* cw = addConnectionWidget(srcWidget, tgtPort);
                                if (cw) {
                                    emit connectionCreated(cw->connection());
                                    emit graphModified();
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void EditorScene::cleanupTempConnection() {
    if (tempConnection_) {
        removeItem(tempConnection_);
        delete tempConnection_;
        tempConnection_ = nullptr;
    }
    dragSourcePort_ = nullptr;
}

void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    auto* item = itemAt(event->scenePos(), QTransform());
    auto* portWidget = dynamic_cast<PortWidget*>(item);

    if (portWidget && event->button() == Qt::LeftButton) {
        if (portWidget->port()->direction() == PortDirection::Output) {
            dragSourcePort_ = portWidget;
            tempConnection_ = new TempConnectionWidget(dragSourcePort_);
            addItem(tempConnection_);
            event->accept();
            return;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (tempConnection_) {
        tempConnection_->updateEndPoint(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (tempConnection_) {
        auto* item = itemAt(event->scenePos(), QTransform());
        auto* targetPort = dynamic_cast<PortWidget*>(item);

        bool connected = false;
        if (targetPort && dragSourcePort_) {
            if (targetPort->port()->direction() == PortDirection::Input &&
                targetPort->port()->parentNode() != dragSourcePort_->port()->parentNode()) {
                if (typesCompatible(dragSourcePort_->port()->type(),
                                    targetPort->port()->type())) {
                    auto* cw = addConnectionWidget(dragSourcePort_, targetPort);
                    if (cw) {
                        connected = true;
                        emit connectionCreated(cw->connection());
                        emit graphModified();
                    }
                }
            }
        }

        if (!connected && typeLister_ && nodeCreator_) {
            // Dropped on blank area → show node picker popup
            QPointF dropPos = event->scenePos();
            // We need to defer showing the dialog because we're in mouseReleaseEvent
            // Use a single-shot timer to avoid re-entrance issues
            QMetaObject::invokeMethod(this, [this, dropPos]() {
                showNodePicker(dropPos);
            }, Qt::QueuedConnection);
        }

        removeItem(tempConnection_);
        delete tempConnection_;
        tempConnection_ = nullptr;
        dragSourcePort_ = nullptr;

        if (connected) {
            event->accept();
            return;
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void EditorScene::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        auto selected = selectedItems();
        for (auto* item : selected) {
            if (auto* connWidget = dynamic_cast<ConnectionWidget*>(item)) {
                removeConnectionWidget(connWidget);
            } else if (auto* nodeWidget = dynamic_cast<NodeWidget*>(item)) {
                if (graph_) {
                    graph_->removeNode(nodeWidget->node());
                }
                removeNodeWidget(nodeWidget->node());
            }
        }
        emit graphModified();
        return;
    }

    QGraphicsScene::keyPressEvent(event);
}

void EditorScene::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->fillRect(rect, ThemeManager::background());

    const int gridSize = 20;
    QPen gridPen(ThemeManager::gridMinor(), 0.5);
    QPen majorGridPen(ThemeManager::gridMajor(), 1.0);

    int left = static_cast<int>(rect.left()) - (static_cast<int>(rect.left()) % gridSize);
    int top = static_cast<int>(rect.top()) - (static_cast<int>(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 500> minorLines;
    QVarLengthArray<QLineF, 100> majorLines;

    for (int x = left; x < static_cast<int>(rect.right()); x += gridSize) {
        if (x % (gridSize * 5) == 0)
            majorLines.append(QLineF(x, rect.top(), x, rect.bottom()));
        else
            minorLines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }
    for (int y = top; y < static_cast<int>(rect.bottom()); y += gridSize) {
        if (y % (gridSize * 5) == 0)
            majorLines.append(QLineF(rect.left(), y, rect.right(), y));
        else
            minorLines.append(QLineF(rect.left(), y, rect.right(), y));
    }

    painter->setPen(gridPen);
    painter->drawLines(minorLines.data(), minorLines.size());
    painter->setPen(majorGridPen);
    painter->drawLines(majorLines.data(), majorLines.size());
}

} // namespace QBlock