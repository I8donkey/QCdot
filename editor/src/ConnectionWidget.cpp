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
    setAcceptHoverEvents(true);
    // Use the source port's data type color (set in paint for hover too)
    QColor typeColor = source_ && source_->port() ? source_->port()->color() : QColor(200, 200, 200);
    setPen(QPen(typeColor, 2.5));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
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
    QColor lineColor = QColor(200, 200, 200);
    if (source_ && source_->port()) {
        lineColor = source_->port()->color();
    }

    QPen pen(lineColor, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if (isUnderMouse() || isSelected()) {
        pen.setWidth(4.0);
        pen.setColor(lineColor.lighter(140));
    }
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path());

    // Draw small arrow dot near target end
    if (target_) {
        QPointF p = path().pointAtPercent(0.95);
        painter->setBrush(lineColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(p, 3.5, 3.5);
    }
}

// ---- TempConnectionWidget ----

TempConnectionWidget::TempConnectionWidget(PortWidget* source, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , source_(source)
{
    setZValue(4);
    QColor typeColor = source_ && source_->port() ? source_->port()->color() : QColor(220, 220, 220);
    QPen pen(typeColor, 2.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setDashPattern({6, 4});
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