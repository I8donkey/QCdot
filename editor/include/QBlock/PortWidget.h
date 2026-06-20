#ifndef QBLOCK_PORT_WIDGET_H
#define QBLOCK_PORT_WIDGET_H

#include <QBlock/Port.h>
#include <QGraphicsEllipseItem>
#include <QPainterPath>

namespace QBlock {

class ConnectionWidget;
class NodeWidget;

/// Visual representation of a port on the node widget.
class PortWidget final : public QGraphicsEllipseItem {
public:
    PortWidget(Port* port, NodeWidget* parent);

    Port* port() const { return port_; }
    NodeWidget* nodeWidget() const { return nodeWidget_; }
    QPointF centerPos() const;

    // Connection management
    void addConnection(ConnectionWidget* conn);
    void removeConnection(ConnectionWidget* conn);
    const QList<ConnectionWidget*>& connections() const { return connections_; }

    enum { Type = QGraphicsItem::UserType + 1 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    Port* port_;
    NodeWidget* nodeWidget_;
    QList<ConnectionWidget*> connections_;
};

} // namespace QBlock

#endif // QBLOCK_PORT_WIDGET_H