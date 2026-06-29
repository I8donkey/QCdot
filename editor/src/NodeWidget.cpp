#include <QBlock/NodeWidget.h>
#include <QBlock/PortWidget.h>
#include <QBlock/ConnectionWidget.h>
#include <QBlock/Translator.h>
#include <QBlock/ThemeManager.h>
#include <QBlock/BuiltinNodes.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QInputDialog>

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
    int inputCount = inputPorts_.size();
    int outputCount = outputPorts_.size();
    int maxCount = std::max(inputCount, outputCount);

    // Position input ports on the left side
    for (int i = 0; i < inputCount; ++i) {
        float y = kHeaderHeight + static_cast<float>(i) * kPortHeight;
        inputPorts_[i]->setPos(0, y);
    }

    // Position output ports on the right side, aligned with input ports
    for (int i = 0; i < outputCount; ++i) {
        float y = kHeaderHeight + static_cast<float>(i) * kPortHeight;
        outputPorts_[i]->setPos(kWidth, y);
    }

    // Suppress unused variable warning
    (void)maxCount;
}

bool NodeWidget::isContainerNode() const {
    const std::string& typeName = node_->typeName();
    return typeName == "QtMainWindow" || typeName == "QtTabWidget" || 
           typeName == "QtHBoxLayout" || typeName == "QtVBoxLayout";
}

void NodeWidget::addContainerItem() {
    const std::string& typeName = node_->typeName();
    
    if (typeName == "QtMainWindow") {
        int widgetCount = 0;
        for (const auto& port : node_->inputs()) {
            if (port->name().find("widget") == 0) {
                widgetCount++;
            }
        }
        std::string newPortName = "widget" + std::to_string(widgetCount + 1);
        node_->addInput(newPortName, DataType::Generic);
    } else if (typeName == "QtTabWidget") {
        int tabCount = 0;
        for (const auto& port : node_->inputs()) {
            if (port->name().find("tab") == 0) {
                tabCount++;
            }
        }
        std::string newPortName = "tab" + std::to_string(tabCount + 1);
        node_->addInput(newPortName, DataType::Generic);
    } else if (typeName == "QtHBoxLayout" || typeName == "QtVBoxLayout") {
        int itemCount = 0;
        for (const auto& port : node_->inputs()) {
            if (port->name().find("item") == 0) {
                itemCount++;
            }
        }
        std::string newPortName = "item" + std::to_string(itemCount + 1);
        node_->addInput(newPortName, DataType::Generic);
    }
    
    refresh();
}

QRectF NodeWidget::boundingRect() const {
    float h = kHeaderHeight + kPadding;
    int portCount = std::max(inputPorts_.size(), outputPorts_.size());
    h += portCount * kPortHeight;
    
    if (isContainerNode()) {
        h += kAddButtonSize + 4;
    }
    
    return QRectF(-kMargin, -kMargin, kWidth + kMargin * 2 + kShadowOffset,
                  h + kMargin * 2 + kShadowOffset);
}

void NodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    QRectF r(0, 0, kWidth, boundingRect().height() - kMargin * 2 - kShadowOffset);
    painter->setRenderHint(QPainter::Antialiasing);

    bool selected = (option->state & QStyle::State_Selected);

    // Soft shadow
    QRectF shadowRect = r.translated(kShadowOffset, kShadowOffset);
    QColor shadowColor = ThemeManager::shadow();
    QPainterPath shadowPath;
    shadowPath.addRoundedRect(shadowRect, 8, 8);
    painter->setBrush(shadowColor);
    painter->setPen(Qt::NoPen);
    painter->drawPath(shadowPath);

    // Background with subtle gradient
    QColor bgColor = selected ? ThemeManager::nodeBgSelected() : ThemeManager::nodeBg();
    QLinearGradient bgGrad(r.topLeft(), r.bottomLeft());
    bgGrad.setColorAt(0, bgColor.lighter(102));
    bgGrad.setColorAt(1, bgColor.darker(102));
    painter->setBrush(bgGrad);
    painter->setPen(QPen(selected ? ThemeManager::nodeBorderSelected() : ThemeManager::nodeBorder(),
                         selected ? 2 : 1));
    painter->drawRoundedRect(r, 8, 8);

    // Header bar with gradient
    QRectF header(r.x(), r.y(), r.width(), kHeaderHeight);
    QLinearGradient headerGrad(header.topLeft(), header.bottomLeft());
    headerGrad.setColorAt(0, ThemeManager::nodeHeaderFrom());
    headerGrad.setColorAt(1, ThemeManager::nodeHeaderTo());
    painter->setBrush(headerGrad);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(header, 8, 8);
    painter->drawRect(QRectF(header.x(), header.y() + 4, header.width(), header.height() - 4));

    // Header highlight
    QRectF highlightRect(header.x(), header.y(), header.width(), 2);
    QLinearGradient highlightGrad(highlightRect.topLeft(), highlightRect.bottomLeft());
    highlightGrad.setColorAt(0, QColor(255, 255, 255, 15));
    highlightGrad.setColorAt(1, QColor(255, 255, 255, 0));
    painter->setBrush(highlightGrad);
    painter->drawRect(highlightRect);

    // Title (translated)
    QString titleKey = QStringLiteral("node.") + QString::fromStdString(node_->typeName());
    QString title = Translator::tr(titleKey);
    if (title == titleKey) {
        title = QString::fromStdString(node_->typeName());
    }

    painter->setFont(QFont("Segoe UI", 10, QFont::Bold));
    painter->setPen(ThemeManager::textPrimary());
    painter->drawText(header.adjusted(8, 0, -8, 0), Qt::AlignVCenter, title);
    
    // Selection glow effect
    if (selected) {
        QPen glowPen(ThemeManager::nodeBorderSelected(), 4);
        glowPen.setColor(QColor(97, 175, 239, 40));
        painter->setPen(glowPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(r.adjusted(-2, -2, 2, 2), 10, 10);
    }
    
    // Draw "+" button for container nodes
    if (isContainerNode()) {
        float buttonY = r.height() - kAddButtonSize - 4;
        float buttonX = (r.width() - kAddButtonSize) / 2;
        QRectF buttonRect(buttonX, buttonY, kAddButtonSize, kAddButtonSize);
        
        if (hoverAddButton_) {
            painter->setBrush(ThemeManager::accentColor());
            painter->setPen(Qt::NoPen);
        } else {
            painter->setBrush(QColor(0, 0, 0, 30));
            painter->setPen(QPen(ThemeManager::textSecondary(), 1));
        }
        painter->drawRoundedRect(buttonRect, 4, 4);
        
        painter->setFont(QFont("Consolas", 12, QFont::Bold));
        painter->setPen(hoverAddButton_ ? Qt::white : ThemeManager::textSecondary());
        painter->drawText(buttonRect, Qt::AlignCenter, QStringLiteral("+"));
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
    QPointF localPos = event->pos();
    
    if (isContainerNode()) {
        QRectF r(0, 0, kWidth, boundingRect().height() - kMargin * 2 - kShadowOffset);
        float buttonY = r.height() - kAddButtonSize - 4;
        float buttonX = (r.width() - kAddButtonSize) / 2;
        QRectF buttonRect(buttonX, buttonY, kAddButtonSize, kAddButtonSize);
        
        if (buttonRect.contains(localPos)) {
            addContainerItem();
            return;
        }
    }
    
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

void NodeWidget::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    QPointF localPos = event->pos();
    bool wasHovering = hoverAddButton_;
    
    if (isContainerNode()) {
        QRectF r(0, 0, kWidth, boundingRect().height() - kMargin * 2 - kShadowOffset);
        float buttonY = r.height() - kAddButtonSize - 4;
        float buttonX = (r.width() - kAddButtonSize) / 2;
        QRectF buttonRect(buttonX, buttonY, kAddButtonSize, kAddButtonSize);
        
        hoverAddButton_ = buttonRect.contains(localPos);
    } else {
        hoverAddButton_ = false;
    }
    
    if (hoverAddButton_ != wasHovering) {
        update();
    }
    
    if (hoverAddButton_) {
        setCursor(Qt::PointingHandCursor);
    } else if (!dragging_) {
        setCursor(Qt::OpenHandCursor);
    }
    
    QGraphicsItem::hoverMoveEvent(event);
}

QVariant NodeWidget::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        // Update all connections attached to this node's ports
        for (auto* pw : inputPorts_) {
            for (auto* conn : pw->connections()) {
                conn->updatePath();
            }
        }
        for (auto* pw : outputPorts_) {
            for (auto* conn : pw->connections()) {
                conn->updatePath();
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void NodeWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    // Try to edit constant node values
    if (auto* ci = dynamic_cast<Builtin::ConstantIntNode*>(node_)) {
        bool ok = false;
        int v = QInputDialog::getInt(
            nullptr,
            Translator::currentLanguage() == "zh" ? QStringLiteral("设置整数值") : QStringLiteral("Set Integer Value"),
            Translator::currentLanguage() == "zh" ? QStringLiteral("输入整数：") : QStringLiteral("Enter integer:"),
            static_cast<int>(ci->value()),
            -2147483647, 2147483647, 1, &ok);
        if (ok) ci->setValue(static_cast<int64_t>(v));
        return;
    }
    if (auto* cf = dynamic_cast<Builtin::ConstantFloatNode*>(node_)) {
        bool ok = false;
        double v = QInputDialog::getDouble(
            nullptr,
            Translator::currentLanguage() == "zh" ? QStringLiteral("设置浮点数值") : QStringLiteral("Set Float Value"),
            Translator::currentLanguage() == "zh" ? QStringLiteral("输入浮点数：") : QStringLiteral("Enter float:"),
            cf->value(),
            -1000000.0, 1000000.0, 6, &ok);
        if (ok) cf->setValue(v);
        return;
    }
    if (auto* cb = dynamic_cast<Builtin::ConstantBoolNode*>(node_)) {
        QStringList items;
        items << (Translator::currentLanguage() == "zh" ? QStringLiteral("是 / true") : QStringLiteral("Yes / true"))
              << (Translator::currentLanguage() == "zh" ? QStringLiteral("否 / false") : QStringLiteral("No / false"));
        bool ok = false;
        QString item = QInputDialog::getItem(
            nullptr,
            Translator::currentLanguage() == "zh" ? QStringLiteral("设置布尔值") : QStringLiteral("Set Boolean Value"),
            Translator::currentLanguage() == "zh" ? QStringLiteral("选择值：") : QStringLiteral("Choose value:"),
            items, cb->value() ? 0 : 1, false, &ok);
        if (ok && !item.isEmpty()) {
            cb->setValue(items.indexOf(item) == 0);
        }
        return;
    }
    if (auto* cs = dynamic_cast<Builtin::ConstantStringNode*>(node_)) {
        bool ok = false;
        QString text = QInputDialog::getText(
            nullptr,
            Translator::currentLanguage() == "zh" ? QStringLiteral("设置字符串") : QStringLiteral("Set String Value"),
            Translator::currentLanguage() == "zh" ? QStringLiteral("输入文本：") : QStringLiteral("Enter text:"),
            QLineEdit::Normal,
            QString::fromStdString(cs->value()), &ok);
        if (ok) cs->setValue(text.toStdString());
        return;
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

} // namespace QBlock