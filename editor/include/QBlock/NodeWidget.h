#ifndef QBLOCK_NODE_WIDGET_H
#define QBLOCK_NODE_WIDGET_H

#include <QBlock/Node.h>
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <memory>

namespace QBlock {

class PortWidget;

/// Visual node widget on the editor canvas.
class NodeWidget final : public QGraphicsItem {
public:
    explicit NodeWidget(Node* node);
    ~NodeWidget() override;

    Node* node() const { return node_; }
    QList<PortWidget*> inputPorts() const { return inputPorts_; }
    QList<PortWidget*> outputPorts() const { return outputPorts_; }

    PortWidget* findPortWidget(const std::string& portName, bool isInput) const;

    void refresh();

    enum { Type = QGraphicsItem::UserType + 3 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void createPortWidgets();
    void layout();

    Node* node_;
    QList<PortWidget*> inputPorts_;
    QList<PortWidget*> outputPorts_;

    static constexpr float kWidth = 160.0f;
    static constexpr float kHeaderHeight = 28.0f;
    static constexpr float kPortHeight = 20.0f;
    static constexpr float kPadding = 8.0f;
    static constexpr float kShadowOffset = 3.0f;
    static constexpr float kMargin = 6.0f;

    bool dragging_ = false;
    QPointF dragStart_;
};

} // namespace QBlock

#endif // QBLOCK_NODE_WIDGET_H