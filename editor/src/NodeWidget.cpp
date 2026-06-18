#include <QBlock/NodeWidget.h>
#include <QBlock/PortWidget.h>
#include <QBlock/Translator.h>
#include <QBlock/ThemeManager.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>

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
    return QRectF(-kMargin, -kMargin, kWidth + kMargin * 2 + kShadowOffset,
                  h + kMargin * 2 + kShadowOffset);
}

void NodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    QRectF r(0, 0, kWidth, boundingRect().height() - kMargin * 2 - kShadowOffset);
    painter->setRenderHint(QPainter::Antialiasing);

    // Shadow
    QRectF shadowRect = r.translated(kShadowOffset, kShadowOffset);
    painter->setBrush(QColor(0, 0, 0, 60));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(shadowRect, 6, 6);

    // Background (theme-aware)
    bool selected = (option->state & QStyle::State_Selected);
    QColor bgColor = selected ? ThemeManager::nodeBgSelected() : ThemeManager::nodeBg();
    painter->setBrush(bgColor);
    painter->setPen(QPen(selected ? ThemeManager::nodeBorderSelected() : ThemeManager::nodeBorder(),
                         selected ? 2 : 1));
    painter->drawRoundedRect(r, 6, 6);

    // Header bar (theme-aware)
    QRectF header(r.x(), r.y(), r.width(), kHeaderHeight);
    QLinearGradient grad(header.topLeft(), header.bottomLeft());
    grad.setColorAt(0, ThemeManager::nodeHeaderFrom());
    grad.setColorAt(1, ThemeManager::nodeHeaderTo());
    painter->setBrush(grad);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(header, 6, 6);
    painter->drawRect(QRectF(header.x(), header.y() + 4, header.width(), header.height() - 4));

    // Title (translated)
    QString titleKey = QStringLiteral("node.") + QString::fromStdString(node_->typeName());
    QString title = Translator::tr(titleKey);
    if (title == titleKey) {
        // fallback to type name if no translation found
        title = QString::fromStdString(node_->typeName());
    }

    painter->setFont(QFont("Consolas", 10, QFont::Bold));
    painter->setPen(ThemeManager::textPrimary());
    painter->drawText(header.adjusted(8, 0, -8, 0), Qt::AlignVCenter, title);

    // Chinese-only mode: if language is zh, only show translated name, no English fallback
    // Skip the small type name subtitle when in Chinese mode, since it's already the translated name
    if (Translator::currentLanguage() != QStringLiteral("zh")) {
        painter->setFont(QFont("Consolas", 7));
        painter->setPen(ThemeManager::textSecondary());
        painter->drawText(QRectF(r.x() + 8, r.y() + kHeaderHeight, r.width() - 16, 14),
                          Qt::AlignVCenter, QString::fromStdString(node_->typeName()));
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
    const auto children = childItems();
    for (auto* child : children) {
        if (auto* pw = dynamic_cast<PortWidget*>(child)) {
            delete pw;
        }
    }
    inputPorts_.clear();
    outputPorts_.clear();
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
        node_->setPosition(static_cast<float>(pos().x()), static_cast<float>(pos().y()));
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

} // namespace QBlock