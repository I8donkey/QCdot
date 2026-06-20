#ifndef QBLOCK_NODE_EDITOR_H
#define QBLOCK_NODE_EDITOR_H

#include "EditorScene.h"
#include <QBlock/NodeGraph.h>
#include <QBlock/ExecutionEngine.h>
#include <QBlock/ThemeManager.h>
#include <QGraphicsView>
#include <QToolBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

namespace QBlock {

/// Main editor widget that provides the full node editing experience.
/// Contains a QGraphicsView, toolbar with settings menu, and manages the EditorScene.
class NodeEditor final : public QWidget {
    Q_OBJECT
public:
    explicit NodeEditor(QWidget* parent = nullptr);
    ~NodeEditor() override;

    // ---- Graph management ----

    NodeGraph* graph() const { return scene_->graph(); }

    /// Create a new blank graph.
    void newGraph();

    /// Load a graph from a .qcd file.
    bool loadFromFile(const QString& filePath);

    /// Save the current graph to a .qcd file.
    bool saveToFile(const QString& filePath) const;

    /// Export the current graph as a .cpp source file.
    bool exportToCpp(const QString& filePath) const;

    // ---- Execution ----

    void executeDataflow();
    void executeSignal(Node* entryNode = nullptr);

    // ---- Node factory ----

    using NodeFactory = std::function<std::unique_ptr<Node>(const std::string& typeName)>;
    void setNodeFactory(NodeFactory factory) { nodeFactory_ = std::move(factory); }
    const NodeFactory& nodeFactory() const { return nodeFactory_; }

    /// List all available node type names from the factory.
    std::vector<std::string> listNodeTypes() const;

    /// Add a node to the graph at a given position.
    Node* addNode(const std::string& typeName, float x, float y);

    // ---- View ----

    QGraphicsView* view() const { return view_; }
    EditorScene* editorScene() const { return scene_; }

    /// Fit the view to show all nodes.
    void fitToScreen();

    /// Toggle dark/light theme.
    void toggleTheme();

    void copyNodes();
    void pasteNodes();

public slots:
    void setLanguage(const QString& lang);

signals:
    void nodeSelected(Node* node);
    void graphModified();
    void languageChanged();

private:
    void setupUI();
    void setupToolbar();
    void setupSettingsMenu(QToolBar* toolbar);
    void applyThemeStyle();
    std::string generateCppCode() const;

    EditorScene* scene_;
    QGraphicsView* view_;
    ExecutionEngine engine_;
    NodeFactory nodeFactory_;
    NodeGraph defaultGraph_;
    QVBoxLayout* layout_;
    QToolBar* toolbar_;
    QAction* langAct_ = nullptr;
    QAction* themeAct_ = nullptr;
    QMenu* settingsMenu_ = nullptr;
};

} // namespace QBlock

#endif // QBLOCK_NODE_EDITOR_H