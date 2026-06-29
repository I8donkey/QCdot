#ifndef QBLOCK_EDITOR_SCENE_H
#define QBLOCK_EDITOR_SCENE_H

#include <QBlock/NodeGraph.h>
#include <QBlock/ThemeManager.h>
#include <QBlock/InlineNodePicker.h>
#include "NodeWidget.h"
#include "PortWidget.h"
#include "ConnectionWidget.h"
#include <QGraphicsScene>
#include <QMetaObject>
#include <QTimer>
#include <unordered_map>
#include <functional>

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
    NodeWidget* findNodeWidgetById(uint64_t id) const;

    // ---- Connection operations ----

    ConnectionWidget* addConnectionWidget(PortWidget* source, PortWidget* target);
    void removeConnectionWidget(ConnectionWidget* widget);

    // ---- Utility ----

    void clear();

    /// Set a callback to list all available node types (for blank-area popup).
    using TypeLister = std::function<std::vector<std::string>()>;
    void setTypeLister(TypeLister lister) { typeLister_ = std::move(lister); }

    /// Set a callback to create/add a node (for blank-area popup).
    using NodeCreator = std::function<Node*(const std::string& type, float x, float y)>;
    void setNodeCreator(NodeCreator creator) { nodeCreator_ = std::move(creator); }

    /// Show the inline node picker at the given scene position.
    void showInlinePicker(const QPointF& scenePos);
    void showContextMenu(const QPointF& scenePos);

signals:
    void nodeSelected(Node* node);
    void connectionCreated(Connection* conn);
    void graphModified();
    void undoRequested();
    void redoRequested();
    void selectAllRequested();

public:
    /// Request undo action
    void requestUndo() { emit undoRequested(); }
    /// Request redo action
    void requestRedo() { emit redoRequested(); }
    /// Request select all nodes
    void requestSelectAll() { emit selectAllRequested(); }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    NodeGraph* graph_ = nullptr;
    std::unordered_map<Node*, NodeWidget*> nodeWidgetMap_;

    // Dragging state for new connections
    PortWidget* dragSourcePort_ = nullptr;
    TempConnectionWidget* tempConnection_ = nullptr;
    ConnectionWidget* pickedUpConnection_ = nullptr;

    // Callbacks for blank-area popup
    TypeLister typeLister_;
    NodeCreator nodeCreator_;

    // Inline node picker
    InlineNodePicker* inlinePicker_ = nullptr;

    void cleanupTempConnection();
    void closeInlinePicker();
};

} // namespace QBlock

#endif // QBLOCK_EDITOR_SCENE_H