#include <QBlock/ConnectionWidget.h>
#include <QBlock/PortWidget.h>
#include <QBlock/ThemeManager.h>
#include <QPainter>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>

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
    painter->setRenderHint(QPainter::Antialiasing);

    QColor lineColor = ThemeManager::connectionLine();
    if (source_ && source_->port()) {
        lineColor = source_->port()->color();
    }

    QPainterPath p = path();
    
    // Draw glow effect when hovered or selected
    if (isUnderMouse() || isSelected()) {
        QPen glowPen(lineColor, 8.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        glowPen.setColor(QColor(lineColor.red(), lineColor.green(), lineColor.blue(), 40));
        painter->setPen(glowPen);
        painter->drawPath(p);
    }

    // Main line with gradient
    QLinearGradient grad(p.pointAtPercent(0), p.pointAtPercent(1));
    grad.setColorAt(0, lineColor.lighter(110));
    grad.setColorAt(1, lineColor.darker(110));
    
    QPen pen(grad, isUnderMouse() || isSelected() ? 3.0 : 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(p);

    // Draw arrowhead near target end
    if (target_) {
        QPointF targetPoint = p.pointAtPercent(0.95);
        QPointF prevPoint = p.pointAtPercent(0.88);
        
        // Calculate direction vector
        QPointF dir = targetPoint - prevPoint;
        double length = sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (length < 0.001) return;
        
        // Normalize direction
        QPointF unitDir = dir / length;
        
        // Perpendicular vector for arrow wings
        QPointF perp(-unitDir.y(), unitDir.x());
        
        // Arrow dimensions
        double arrowSize = 7.0;
        double arrowWingSpan = 4.0;
        
        // Arrow points
        QPointF arrowTip = targetPoint;
        QPointF arrowLeft = arrowTip - arrowSize * unitDir + arrowWingSpan * perp;
        QPointF arrowRight = arrowTip - arrowSize * unitDir - arrowWingSpan * perp;
        
        // Draw arrow triangle
        QPainterPath arrowPath;
        arrowPath.moveTo(arrowTip);
        arrowPath.lineTo(arrowLeft);
        arrowPath.lineTo(arrowRight);
        arrowPath.closeSubpath();
        
        painter->setBrush(lineColor);
        painter->setPen(Qt::NoPen);
        painter->drawPath(arrowPath);
    }
}

void ConnectionWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (!target_) {
        QGraphicsPathItem::mousePressEvent(event);
        return;
    }

    QPainterPath p = path();
    QPointF targetPoint = p.pointAtPercent(0.95);
    QPointF prevPoint = p.pointAtPercent(0.88);
    
    QPointF dir = targetPoint - prevPoint;
    double length = sqrt(dir.x() * dir.x() + dir.y() * dir.y());
    if (length < 0.001) {
        QGraphicsPathItem::mousePressEvent(event);
        return;
    }
    
    QPointF unitDir = dir / length;
    QPointF perp(-unitDir.y(), unitDir.x());
    
    double arrowSize = 7.0;
    double arrowWingSpan = 4.0;
    
    QPointF arrowTip = targetPoint;
    QPointF arrowLeft = arrowTip - arrowSize * unitDir + arrowWingSpan * perp;
    QPointF arrowRight = arrowTip - arrowSize * unitDir - arrowWingSpan * perp;
    
    QPainterPath arrowPath;
    arrowPath.moveTo(arrowTip);
    arrowPath.lineTo(arrowLeft);
    arrowPath.lineTo(arrowRight);
    arrowPath.closeSubpath();
    
    QPointF localPos = event->pos();
    if (arrowPath.contains(localPos)) {
        emit arrowDragged(this, event->scenePos());
        event->accept();
        return;
    }
    
    QGraphicsPathItem::mousePressEvent(event);
}

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