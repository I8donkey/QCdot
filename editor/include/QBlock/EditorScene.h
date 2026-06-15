#ifndef QBLOCK_EDITOR_SCENE_H
#define QBLOCK_EDITOR_SCENE_H

#include <QBlock/NodeGraph.h>
#include "NodeWidget.h"
#include "PortWidget.h"
#include "ConnectionWidget.h"
#include <QGraphicsScene>
#include <unordered_map>

namespace QBlock {

/// The QGraphicsScene that manages the visual editor canvas.
class EditorScene final : public QGraphicsScene {
    Q_OBJECT
public:
    explicit EditorScene(QObject* parent = nullptr);
    ~EditorScene() override;

    // ---- Graph binding ----

    /// Set the underlying graph and rebuild all visuals.
    void setGraph(NodeGraph* graph);
    NodeGraph* graph() const { return graph_; }

    /// Rebuild all visual widgets from the current graph.
    void rebuildFromGraph();

    /// Apply current visual positions back to the graph nodes.
    void syncPositionsToGraph();

    // ---- Node operations ----

    NodeWidget* addNodeWidget(Node* node);
    void removeNodeWidget(Node* node);
    NodeWidget* findNodeWidget(Node* node) const;

    // ---- Connection operations ----

    ConnectionWidget* addConnectionWidget(PortWidget* source, PortWidget* target);
    void removeConnectionWidget(ConnectionWidget* widget);

    // ---- Utility ----

    void clear();

    // ---- Port drag handling ----
    // Called by PortWidget during connection dragging
    void startPortDrag(PortWidget* source);
    void updatePortDrag(const QPointF& scenePos);
    void endPortDrag(const QPointF& scenePos);

signals:
    void nodeSelected(Node* node);
    void connectionCreated(Connection* conn);
    void graphModified();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    NodeGraph* graph_ = nullptr;
    std::unordered_map<Node*, NodeWidget*> nodeWidgetMap_;

    // Dragging state for new connections
    PortWidget* dragSourcePort_ = nullptr;
    TempConnectionWidget* tempConnection_ = nullptr;

    void cleanupTempConnection();
};

} // namespace QBlock

#endif // QBLOCK_EDITOR_SCENE_H