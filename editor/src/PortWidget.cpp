#include <QBlock/PortWidget.h>
#include <QBlock/ConnectionWidget.h>
#include <QBlock/NodeWidget.h>
#include <QCursor>
#include <QPainter>
#include <QPen>

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

void PortWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QColor c = port_->color();
    painter->setBrush(QBrush(isUnderMouse() ? c.lighter(150) : c));
    painter->setPen(QPen(c.darker(150), 1.5));
    painter->drawEllipse(rect());

    // Port label
    painter->setFont(QFont("Consolas", 8));
    painter->setPen(Qt::white);
    if (port_->isInput()) {
        painter->drawText(QPointF(10, 4), port_->displayName());
    } else {
        QString label = port_->displayName();
        int textWidth = painter->fontMetrics().horizontalAdvance(label);
        painter->drawText(QPointF(-textWidth - 10, 4), label);
    }
}

} // namespace QBlock