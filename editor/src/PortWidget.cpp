#include <QBlock/PortWidget.h>
#include <QBlock/ConnectionWidget.h>
#include <QBlock/NodeWidget.h>
#include <QBlock/Translator.h>
#include <QBlock/ThemeManager.h>
#include <QCursor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

namespace QBlock {

PortWidget::PortWidget(Port* port, NodeWidget* parent)
    : QGraphicsEllipseItem(-6, -6, 12, 12, parent)
    , port_(port)
    , nodeWidget_(parent)
{
    setAcceptHoverEvents(true);
    setCursor(QCursor(Qt::CrossCursor));
    setZValue(10);

    QColor c = port->color();
    setBrush(QBrush(c.darker(130)));
    setPen(QPen(c.lighter(150), 2));
}

QPointF PortWidget::centerPos() const {
    return mapToScene(0, 0);
}

void PortWidget::addConnection(ConnectionWidget* conn) {
    if (!connections_.contains(conn))
        connections_.append(conn);
}

void PortWidget::removeConnection(ConnectionWidget* conn) {
    connections_.removeAll(conn);
}

bool PortWidget::isRemovable() const {
    const std::string& name = port_->name();
    return (name.find("widget") == 0 || name.find("tab") == 0 || name.find("item") == 0);
}

QRectF PortWidget::boundingRect() const {
    QRectF base = QGraphicsEllipseItem::boundingRect();

    QString labelText;
    std::string portName = port_->name();
    
    if (portName.find("widget") == 0) {
        QString baseLabel = Translator::tr("port.widget");
        if (baseLabel != "port.widget") {
            labelText = baseLabel + QString::fromStdString(portName.substr(6));
        } else {
            labelText = QString::fromStdString(portName);
        }
    } else if (portName.find("tab") == 0) {
        QString baseLabel = Translator::tr("port.tab");
        if (baseLabel != "port.tab") {
            labelText = baseLabel + QString::fromStdString(portName.substr(3));
        } else {
            labelText = QString::fromStdString(portName);
        }
    } else if (portName.find("item") == 0) {
        QString baseLabel = Translator::tr("port.item");
        if (baseLabel != "port.item") {
            labelText = baseLabel + QString::fromStdString(portName.substr(4));
        } else {
            labelText = QString::fromStdString(portName);
        }
    } else {
        QString prefix = QStringLiteral("port.");
        labelText = Translator::tr(prefix + QString::fromStdString(portName));
        if (labelText.startsWith(prefix))
            labelText = QString::fromStdString(portName);
    }

    QFontMetrics fm(QFont("Consolas", 8));
    int textWidth = fm.horizontalAdvance(labelText) + 8;

    int extraWidth = 0;
    if (isRemovable()) {
        extraWidth = kDeleteButtonSize + 4;
    }

    if (port_->isInput()) {
        return base.united(QRectF(base.left(), base.top(), base.width() + textWidth + extraWidth, base.height()));
    } else {
        return base.united(QRectF(base.left() - textWidth, base.top(), base.width() + textWidth, base.height()));
    }
}

QPainterPath PortWidget::shape() const {
    QPainterPath path;
    path.addEllipse(-8, -8, 16, 16);
    
    // Add delete button hit area for removable input ports
    if (isRemovable() && port_->isInput()) {
        QFontMetrics fm(QFont("Consolas", 8));
        QString label;
        std::string portName = port_->name();
        
        if (portName.find("widget") == 0) {
            QString baseLabel = Translator::tr("port.widget");
            label = (baseLabel != "port.widget") ? baseLabel + QString::fromStdString(portName.substr(6)) : QString::fromStdString(portName);
        } else if (portName.find("tab") == 0) {
            QString baseLabel = Translator::tr("port.tab");
            label = (baseLabel != "port.tab") ? baseLabel + QString::fromStdString(portName.substr(3)) : QString::fromStdString(portName);
        } else if (portName.find("item") == 0) {
            QString baseLabel = Translator::tr("port.item");
            label = (baseLabel != "port.item") ? baseLabel + QString::fromStdString(portName.substr(4)) : QString::fromStdString(portName);
        } else {
            label = QString::fromStdString(portName);
        }
        
        float buttonX = 12 + fm.horizontalAdvance(label);
        float buttonY = -kDeleteButtonSize / 2 - 1;
        path.addRect(buttonX, buttonY, kDeleteButtonSize, kDeleteButtonSize);
    }
    
    return path;
}

void PortWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QColor c = port_->color();
    painter->setRenderHint(QPainter::Antialiasing);

    // Glow effect when hovered
    if (isUnderMouse()) {
        QRadialGradient glowGrad(rect().center(), kRadius * 2);
        glowGrad.setColorAt(0, c.lighter(200));
        glowGrad.setColorAt(0.5, c.lighter(150));
        glowGrad.setColorAt(1, QColor(c.red(), c.green(), c.blue(), 0));
        painter->setBrush(glowGrad);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect().adjusted(-4, -4, 4, 4));
    }

    // Outer ring
    painter->setPen(QPen(c.darker(150), 1.5));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(rect());

    // Inner fill with gradient
    QRadialGradient fillGrad(rect().center(), kRadius);
    fillGrad.setColorAt(0, c.lighter(130));
    fillGrad.setColorAt(1, c.darker(110));
    painter->setBrush(fillGrad);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(rect().adjusted(1, 1, -1, -1));

    // Highlight spot
    QRadialGradient highlightGrad(QPointF(rect().left() + 2, rect().top() + 2), kRadius / 2);
    highlightGrad.setColorAt(0, QColor(255, 255, 255, 80));
    highlightGrad.setColorAt(1, QColor(255, 255, 255, 0));
    painter->setBrush(highlightGrad);
    painter->drawEllipse(rect().adjusted(1, 1, -1, -1));

    QString label;
    QString prefix = QStringLiteral("port.");
    std::string portName = port_->name();
    
    if (portName.find("widget") == 0) {
        QString baseLabel = Translator::tr("port.widget");
        if (baseLabel != "port.widget") {
            label = baseLabel + QString::fromStdString(portName.substr(6));
        } else {
            label = QString::fromStdString(portName);
        }
    } else if (portName.find("tab") == 0) {
        QString baseLabel = Translator::tr("port.tab");
        if (baseLabel != "port.tab") {
            label = baseLabel + QString::fromStdString(portName.substr(3));
        } else {
            label = QString::fromStdString(portName);
        }
    } else if (portName.find("item") == 0) {
        QString baseLabel = Translator::tr("port.item");
        if (baseLabel != "port.item") {
            label = baseLabel + QString::fromStdString(portName.substr(4));
        } else {
            label = QString::fromStdString(portName);
        }
    } else {
        label = Translator::tr(prefix + QString::fromStdString(portName));
        if (label.startsWith(prefix)) {
            label = QString::fromStdString(portName);
        }
    }

    QFontMetrics fm(QFont("Segoe UI", 8));
    int textX = 10;
    
    // Draw delete button for removable input ports (on the right side of label)
    if (isRemovable() && port_->isInput()) {
        float buttonX = 12 + fm.horizontalAdvance(label);
        float buttonY = -kDeleteButtonSize / 2 - 1;
        QRectF buttonRect(buttonX, buttonY, kDeleteButtonSize, kDeleteButtonSize);
        
        // Button shadow
        QRectF shadowRect = buttonRect.translated(1, 1);
        painter->setBrush(QColor(0, 0, 0, 30));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(shadowRect, 4, 4);
        
        // Draw button background with gradient effect
        QLinearGradient gradient(buttonRect.topLeft(), buttonRect.bottomLeft());
        if (hoverDeleteButton_) {
            gradient.setColorAt(0, QColor(239, 68, 68));
            gradient.setColorAt(1, QColor(220, 38, 38));
        } else {
            gradient.setColorAt(0, QColor(179, 68, 68));
            gradient.setColorAt(1, QColor(153, 27, 27));
        }
        painter->setBrush(gradient);
        painter->setPen(QPen(QColor(100, 30, 30), 1));
        painter->drawRoundedRect(buttonRect, 4, 4);
        
        // Draw × symbol with slight shadow
        painter->setFont(QFont("Segoe UI", 11, QFont::Bold));
        painter->setPen(QColor(0, 0, 0, 30));
        painter->drawText(buttonRect.adjusted(0, 1, 0, 1), Qt::AlignCenter, QStringLiteral("×"));
        painter->setPen(Qt::white);
        painter->drawText(buttonRect, Qt::AlignCenter, QStringLiteral("×"));
    }

    painter->setFont(QFont("Segoe UI", 8));
    painter->setPen(ThemeManager::textPrimary());
    if (port_->isInput()) {
        painter->drawText(QPointF(textX, 4), label);
    } else {
        int textWidth = painter->fontMetrics().horizontalAdvance(label);
        painter->drawText(QPointF(-textWidth - 10, 4), label);
    }
}

void PortWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (isRemovable() && port_->isInput()) {
        QPointF localPos = event->pos();
        
        QFontMetrics fm(QFont("Consolas", 8));
        QString label;
        std::string portName = port_->name();
        
        if (portName.find("widget") == 0) {
            QString baseLabel = Translator::tr("port.widget");
            label = (baseLabel != "port.widget") ? baseLabel + QString::fromStdString(portName.substr(6)) : QString::fromStdString(portName);
        } else if (portName.find("tab") == 0) {
            QString baseLabel = Translator::tr("port.tab");
            label = (baseLabel != "port.tab") ? baseLabel + QString::fromStdString(portName.substr(3)) : QString::fromStdString(portName);
        } else if (portName.find("item") == 0) {
            QString baseLabel = Translator::tr("port.item");
            label = (baseLabel != "port.item") ? baseLabel + QString::fromStdString(portName.substr(4)) : QString::fromStdString(portName);
        } else {
            label = QString::fromStdString(portName);
        }
        
        float buttonX = 12 + fm.horizontalAdvance(label);
        float buttonY = -kDeleteButtonSize / 2 - 1;
        QRectF buttonRect(buttonX, buttonY, kDeleteButtonSize, kDeleteButtonSize);
        
        if (buttonRect.contains(localPos)) {
            Node* node = nodeWidget_->node();
            node->removeInput(port_->name());
            nodeWidget_->refresh();
            event->accept();
            return;
        }
    }
    QGraphicsEllipseItem::mousePressEvent(event);
}

void PortWidget::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (isRemovable() && port_->isInput()) {
        QPointF localPos = event->pos();
        
        QFontMetrics fm(QFont("Consolas", 8));
        QString label;
        std::string portName = port_->name();
        
        if (portName.find("widget") == 0) {
            QString baseLabel = Translator::tr("port.widget");
            label = (baseLabel != "port.widget") ? baseLabel + QString::fromStdString(portName.substr(6)) : QString::fromStdString(portName);
        } else if (portName.find("tab") == 0) {
            QString baseLabel = Translator::tr("port.tab");
            label = (baseLabel != "port.tab") ? baseLabel + QString::fromStdString(portName.substr(3)) : QString::fromStdString(portName);
        } else if (portName.find("item") == 0) {
            QString baseLabel = Translator::tr("port.item");
            label = (baseLabel != "port.item") ? baseLabel + QString::fromStdString(portName.substr(4)) : QString::fromStdString(portName);
        } else {
            label = QString::fromStdString(portName);
        }
        
        float buttonX = 12 + fm.horizontalAdvance(label);
        float buttonY = -kDeleteButtonSize / 2 - 1;
        QRectF buttonRect(buttonX, buttonY, kDeleteButtonSize, kDeleteButtonSize);
        
        bool wasHovering = hoverDeleteButton_;
        hoverDeleteButton_ = buttonRect.contains(localPos);
        
        if (hoverDeleteButton_ != wasHovering) {
            update();
        }
        
        if (hoverDeleteButton_) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(QCursor(Qt::CrossCursor));
        }
    }
    QGraphicsEllipseItem::hoverMoveEvent(event);
}

void PortWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hoverDeleteButton_ = false;
    setCursor(QCursor(Qt::CrossCursor));
    update();
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

} // namespace QBlock