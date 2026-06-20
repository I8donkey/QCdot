#include <QBlock/EditorScene.h>
#include <QBlock/ConnectionWidget.h>
#include <QBlock/PortWidget.h>
#include <QBlock/Translator.h>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QApplication>
#include <QTimer>
#include <QMimeData>

namespace QBlock {

EditorScene::EditorScene(QObject* parent)
    : QGraphicsScene(parent)
    , pickedUpConnection_(nullptr)
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
        NodeWidget* widget = it->second;

        // First, remove all connections attached to this node's ports
        // Collect all connection widgets first to avoid modifying list while iterating
        QList<ConnectionWidget*> connsToRemove;
        for (auto* port : widget->inputPorts()) {
            for (auto* conn : port->connections()) {
                if (!connsToRemove.contains(conn)) {
                    connsToRemove.append(conn);
                }
            }
        }
        for (auto* port : widget->outputPorts()) {
            for (auto* conn : port->connections()) {
                if (!connsToRemove.contains(conn)) {
                    connsToRemove.append(conn);
                }
            }
        }

        // Remove all collected connections
        for (auto* conn : connsToRemove) {
            removeConnectionWidget(conn);
        }

        // Now safe to remove the node widget
        removeItem(widget);
        delete widget;
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

void EditorScene::cleanupTempConnection() {
    if (tempConnection_) {
        removeItem(tempConnection_);
        delete tempConnection_;
        tempConnection_ = nullptr;
    }
    // Safety: if a picked-up connection is still hidden, restore it
    if (pickedUpConnection_) {
        pickedUpConnection_->setVisible(true);
        pickedUpConnection_ = nullptr;
    }
    dragSourcePort_ = nullptr;
}

void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (inlinePicker_) {
        QGraphicsItem* itemAtPos = itemAt(event->scenePos(), QTransform());
        if (!itemAtPos || !dynamic_cast<InlineNodePicker*>(itemAtPos)) {
            closeInlinePicker();
        }
    }

    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    // Find item at click position - use a small tolerance for easier clicking on ports
    QList<QGraphicsItem*> itemsAtPos = items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);

    // Check if we hit a port widget (prefer port over node since port is on top)
    PortWidget* hitPort = nullptr;
    for (auto* item : itemsAtPos) {
        if (auto* pw = dynamic_cast<PortWidget*>(item)) {
            // Verify click is actually within the port circle (not just boundingRect)
            QPointF localPt = pw->mapFromScene(event->scenePos());
            // The port is an ellipse at (-6,-6) to (6,6) - use a slightly larger radius for easier clicking
            if (localPt.x() * localPt.x() + localPt.y() * localPt.y() <= 100.0) {
                hitPort = pw;
                break;
            }
        }
    }

    if (hitPort && hitPort->port()->direction() == PortDirection::Output) {
        dragSourcePort_ = hitPort;

        pickedUpConnection_ = nullptr;
        const auto& existing = dragSourcePort_->connections();
        if (!existing.empty()) {
            pickedUpConnection_ = existing.front();
            if (pickedUpConnection_) {
                pickedUpConnection_->setVisible(false);
            }
        }

        tempConnection_ = new TempConnectionWidget(dragSourcePort_);
        addItem(tempConnection_);
        event->accept();
        return;
    }

    if (hitPort && hitPort->port()->direction() == PortDirection::Input) {
        dragSourcePort_ = hitPort;

        pickedUpConnection_ = nullptr;
        const auto& existing = dragSourcePort_->connections();
        if (!existing.empty()) {
            pickedUpConnection_ = existing.front();
            if (pickedUpConnection_) {
                pickedUpConnection_->setVisible(false);
            }
        }

        tempConnection_ = new TempConnectionWidget(dragSourcePort_);
        addItem(tempConnection_);
        event->accept();
        return;
    }

    // If hit a NodeWidget's body (not a port), let it handle its own movement
    if (hitPort) {
        // Clicked on input port - let scene handle it normally
        event->accept();
        return;
    }

    QGraphicsScene::mousePressEvent(event);
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (tempConnection_ && dragSourcePort_) {
        tempConnection_->updateEndPoint(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (tempConnection_ && dragSourcePort_) {
        bool connected = false;

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);
        for (auto* item : itemsAtPos) {
            auto* targetPort = dynamic_cast<PortWidget*>(item);
            if (targetPort && targetPort != dragSourcePort_) {
                bool isSourceOutput = (dragSourcePort_->port()->direction() == PortDirection::Output);
                bool isTargetInput = (targetPort->port()->direction() == PortDirection::Input);

                if (targetPort->port()->parentNode() != dragSourcePort_->port()->parentNode()) {
                    if (typesCompatible(dragSourcePort_->port()->type(), targetPort->port()->type())) {
                        PortWidget* srcPort = nullptr;
                        PortWidget* tgtPort = nullptr;

                        if (isSourceOutput && isTargetInput) {
                            srcPort = dragSourcePort_;
                            tgtPort = targetPort;
                        } else if (!isSourceOutput && !isTargetInput) {
                            srcPort = targetPort;
                            tgtPort = dragSourcePort_;
                        } else {
                            continue;
                        }

                        if (pickedUpConnection_ && pickedUpConnection_->targetPort() != tgtPort) {
                            removeConnectionWidget(pickedUpConnection_);
                            pickedUpConnection_ = nullptr;
                        } else if (pickedUpConnection_ && pickedUpConnection_->targetPort() == tgtPort) {
                            pickedUpConnection_->setVisible(true);
                            pickedUpConnection_ = nullptr;
                            connected = true;
                            break;
                        }
                        auto* cw = addConnectionWidget(srcPort, tgtPort);
                        if (cw) {
                            connected = true;
                            emit connectionCreated(cw->connection());
                            emit graphModified();
                            break;
                        }
                    }
                }
            }
        }

        if (!connected && typeLister_ && nodeCreator_) {
            QPointF dropPos = event->scenePos();
            if (pickedUpConnection_) {
                removeConnectionWidget(pickedUpConnection_);
                pickedUpConnection_ = nullptr;
                emit graphModified();
            }
            QTimer::singleShot(10, this, [this, dropPos]() {
                showInlinePicker(dropPos);
            });
        } else if (!connected && pickedUpConnection_) {
            pickedUpConnection_->setVisible(true);
            pickedUpConnection_ = nullptr;
        }

        cleanupTempConnection();
        event->accept();
        return;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void EditorScene::showInlinePicker(const QPointF& scenePos) {
    if (!typeLister_ || !nodeCreator_) return;

    closeInlinePicker();

    DataType srcType = DataType::Generic;
    if (dragSourcePort_) {
        srcType = dragSourcePort_->port()->type();
    }

    inlinePicker_ = new InlineNodePicker(this, scenePos, srcType, typeLister_);

    connect(inlinePicker_, &InlineNodePicker::nodeSelected, this, [this, scenePos](const std::string& type) {
        if (!type.empty()) {
            float nx = static_cast<float>(scenePos.x());
            float ny = static_cast<float>(scenePos.y());
            Node* newNode = nodeCreator_(type, nx, ny);

            if (newNode && dragSourcePort_) {
                auto* newNodeWidget = findNodeWidget(newNode);
                if (newNodeWidget) {
                    for (const auto& input : newNode->inputs()) {
                        if (typesCompatible(dragSourcePort_->port()->type(), input->type())) {
                            auto* tgtPort = newNodeWidget->findPortWidget(input->name(), true);
                            if (tgtPort) {
                                auto* cw = addConnectionWidget(dragSourcePort_, tgtPort);
                                if (cw) {
                                    emit connectionCreated(cw->connection());
                                    emit graphModified();
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        closeInlinePicker();
    });
}

void EditorScene::closeInlinePicker() {
    if (inlinePicker_) {
        delete inlinePicker_;
        inlinePicker_ = nullptr;
    }
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

void EditorScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qblock-node-type")) {
        event->acceptProposedAction();
    }
}

void EditorScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qblock-node-type")) {
        event->acceptProposedAction();
    }
}

void EditorScene::dropEvent(QGraphicsSceneDragDropEvent* event) {
    if (!event->mimeData()->hasFormat("application/x-qblock-node-type")) return;

    QString typeName = QString::fromUtf8(event->mimeData()->data("application/x-qblock-node-type"));
    if (typeName.isEmpty()) return;

    if (!nodeCreator_) return;

    QPointF scenePos = event->scenePos();
    float x = static_cast<float>(scenePos.x());
    float y = static_cast<float>(scenePos.y());

    Node* newNode = nodeCreator_(typeName.toStdString(), x, y);
    if (newNode) {
        emit graphModified();
    }

    event->acceptProposedAction();
}

} // namespace QBlock
