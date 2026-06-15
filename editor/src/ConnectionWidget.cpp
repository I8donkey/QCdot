#include <QBlock/ConnectionWidget.h>
#include <QBlock/PortWidget.h>
#include <QPainter>
#include <QtMath>

namespace QBlock {

ConnectionWidget::ConnectionWidget(PortWidget* source, PortWidget* target,
                                   Connection* conn, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , source_(source)
    , target_(target)
    , conn_(conn)
{
    setZValue(5);
    setPen(QPen(Qt::white, 2.0));
    updatePath();

    if (source_)
        source_->addConnection(this);
    if (target_)
        target_->addConnection(this);
}

void ConnectionWidget::updatePath() {
    if (!source_ || !target_) return;

    QPointF start = source_->centerPos();
    QPointF end = target_->centerPos();

    QPainterPath p;
    p.moveTo(start);

    // Bezier control points
    float dx = std::abs(end.x() - start.x()) * 0.5f;
    float dy = dx * 0.3f;
    QPointF c1(start.x() + dx, start.y() + dy);
    QPointF c2(end.x() - dx, end.y() - dy);

    p.cubicTo(c1, c2, end);
    setPath(p);
}

void ConnectionWidget::setTarget(PortWidget* target) {
    if (target_) {
        target_->removeConnection(this);
    }
    target_ = target;
    if (target_) {
        target_->addConnection(this);
    }
    updatePath();
}

void ConnectionWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QColor lineColor = Qt::white;
    if (source_ && source_->port()) {
        lineColor = source_->port()->color();
    }

    QPen pen(lineColor, 2.0, Qt::SolidLine, Qt::RoundCap);
    if (isUnderMouse()) {
        pen.setWidth(3.5);
        pen.setColor(lineColor.lighter(150));
    }
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path());

    // Draw arrow at target
    if (target_) {
        QPointF end = target_->centerPos();
        float t = 0.95;
        QPointF p = path().pointAtPercent(t);
        painter->setBrush(lineColor);
        painter->drawEllipse(p, 3, 3);
    }
}

// ---- TempConnectionWidget ----

TempConnectionWidget::TempConnectionWidget(PortWidget* source, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , source_(source)
{
    setZValue(4);
    QPen pen(Qt::white, 2.0, Qt::DashLine);
    pen.setDashPattern({4, 4});
    setPen(pen);
}

void TempConnectionWidget::updateEndPoint(const QPointF& pos) {
    endPoint_ = pos;
    if (!source_) return;

    QPointF start = source_->centerPos();
    float dx = std::abs(pos.x() - start.x()) * 0.5f;

    QPainterPath p;
    p.moveTo(start);
    p.cubicTo(start + QPointF(dx, 0), pos - QPointF(dx, 0), pos);
    setPath(p);
}

void TempConnectionWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setPen(pen());
    painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path());
}

} // namespace QBlock