#ifndef QBLOCK_CONNECTION_WIDGET_H
#define QBLOCK_CONNECTION_WIDGET_H

#include <QBlock/Connection.h>
#include <QGraphicsPathItem>

namespace QBlock {

class PortWidget;

/// Visual bezier curve connection between two ports.
class ConnectionWidget final : public QGraphicsPathItem {
public:
    ConnectionWidget(PortWidget* source, PortWidget* target, Connection* conn = nullptr,
                     QGraphicsItem* parent = nullptr);

    PortWidget* sourcePort() const { return source_; }
    PortWidget* targetPort() const { return target_; }
    Connection* connection() const { return conn_; }

    void updatePath();
    void setTarget(PortWidget* target);

    enum { Type = QGraphicsItem::UserType + 2 };
    int type() const override { return Type; }

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    PortWidget* source_;
    PortWidget* target_;
    Connection* conn_;
};

/// Temporary connection drawn while dragging from a port.
class TempConnectionWidget final : public QGraphicsPathItem {
public:
    explicit TempConnectionWidget(PortWidget* source, QGraphicsItem* parent = nullptr);
    void updateEndPoint(const QPointF& pos);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    PortWidget* source_;
    QPointF endPoint_;
};

} // namespace QBlock

#endif // QBLOCK_CONNECTION_WIDGET_H