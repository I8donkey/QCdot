#include <QBlock/NodeWidget.h>
#include <QBlock/PortWidget.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

namespace QBlock {

NodeWidget::NodeWidget(Node* node)
    : node_(node)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
    setZValue(1);
    createPortWidgets();
    setPos(node->posX(), node->posY());
}

NodeWidget::~NodeWidget() = default;

void NodeWidget::createPortWidgets() {
    inputPorts_.clear();
    outputPorts_.clear();

    for (const auto& port : node_->inputs()) {
        auto* pw = new PortWidget(port.get(), this);
        inputPorts_.append(pw);
    }
    for (const auto& port : node_->outputs()) {
        auto* pw = new PortWidget(port.get(), this);
        outputPorts_.append(pw);
    }
    layout();
}

void NodeWidget::layout() {
    float y = kHeaderHeight;

    for (auto* pw : inputPorts_) {
        pw->setPos(0, y);
        y += kPortHeight;
    }

    // If we have both inputs and outputs, add a separator
    if (!inputPorts_.isEmpty() && !outputPorts_.isEmpty())
        y += kPadding;

    for (auto* pw : outputPorts_) {
        pw->setPos(kWidth, y);
        y += kPortHeight;
    }
}

QRectF NodeWidget::boundingRect() const {
    float h = kHeaderHeight + kPadding;
    h += std::max(inputPorts_.size(), outputPorts_.size()) * kPortHeight;
    return QRectF(0, 0, kWidth, h);
}

void NodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    QRectF r = boundingRect();
    painter->setRenderHint(QPainter::Antialiasing);

    // Shadow
    QRectF shadowRect = r.translated(3, 3);
    painter->setBrush(QColor(0, 0, 0, 60));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(shadowRect, 6, 6);

    // Background
    bool selected = (option->state & QStyle::State_Selected);
    QColor bgColor = selected ? QColor(55, 55, 70) : QColor(40, 40, 50);
    painter->setBrush(bgColor);
    painter->setPen(QPen(selected ? QColor(100, 140, 255) : QColor(70, 70, 80), selected ? 2 : 1));
    painter->drawRoundedRect(r, 6, 6);

    // Header bar
    QRectF header(r.x(), r.y(), r.width(), kHeaderHeight);
    QLinearGradient grad(header.topLeft(), header.bottomLeft());
    grad.setColorAt(0, QColor(60, 60, 80));
    grad.setColorAt(1, QColor(45, 45, 60));
    painter->setBrush(grad);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(header, 6, 6);
    // Only round top corners
    painter->drawRect(QRectF(header.x(), header.y() + 4, header.width(), header.height() - 4));

    // Title
    painter->setFont(QFont("Consolas", 10, QFont::Bold));
    painter->setPen(Qt::white);
    painter->drawText(header.adjusted(8, 0, -8, 0), Qt::AlignVCenter,
                      QString::fromStdString(node_->label()));

    // Type name (smaller, below header)
    painter->setFont(QFont("Consolas", 7));
    painter->setPen(QColor(150, 150, 150));
    painter->drawText(QRectF(r.x() + 8, r.y() + kHeaderHeight, r.width() - 16, 14),
                      Qt::AlignVCenter,
                      QString::fromStdString(node_->typeName()));

    // Port areas
    painter->setFont(QFont("Consolas", 8));
    float portAreaY = kHeaderHeight + 14;
    for (auto* pw : inputPorts_) {
        Q_UNUSED(pw);
        portAreaY += kPortHeight;
    }
    portAreaY += kPadding;
    for (auto* pw : outputPorts_) {
        Q_UNUSED(pw);
        portAreaY += kPortHeight;
    }
}

PortWidget* NodeWidget::findPortWidget(const std::string& portName, bool isInput) const {
    const auto& list = isInput ? inputPorts_ : outputPorts_;
    for (auto* pw : list) {
        if (pw->port()->name() == portName)
            return pw;
    }
    return nullptr;
}

void NodeWidget::refresh() {
    inputPorts_.clear();
    outputPorts_.clear();
    // Remove old port items
    const auto children = childItems();
    for (auto* child : children) {
        if (auto* pw = dynamic_cast<PortWidget*>(child)) {
            delete pw;
        }
    }
    createPortWidgets();
    update();
}

void NodeWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    dragging_ = true;
    dragStart_ = event->scenePos();
    setCursor(Qt::ClosedHandCursor);
    QGraphicsItem::mousePressEvent(event);
}

void NodeWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (dragging_) {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void NodeWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (dragging_) {
        dragging_ = false;
        setCursor(Qt::OpenHandCursor);
        // Update node position
        node_->setPosition(static_cast<float>(pos().x()), static_cast<float>(pos().y()));
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

} // namespace QBlock