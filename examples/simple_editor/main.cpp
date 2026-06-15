#include <QApplication>
#include <QBlock/NodeEditor.h>
#include <QBlock/BuiltinNodes.h>
#include "NodePalette.h"
#include <QMainWindow>
#include <QDockWidget>

/// ============================================================================
/// QBlock Simple Editor Example
///
/// A minimal graphical programming editor demonstrating the QBlock engine.
/// Features:
///   - Dark-themed node editor canvas
///   - Node palette with built-in nodes
///   - Double-click node creation
///   - Connection creation via port dragging
///   - Dataflow and Signal execution modes
///   - Save/Load graph to JSON
/// ============================================================================

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("QBlock - Visual Programming Editor");
    mainWindow.resize(1280, 800);
    mainWindow.setStyleSheet("QMainWindow { background: #1c1c22; }");

    // Create the node editor as central widget
    auto* editor = new QBlock::NodeEditor(&mainWindow);
    mainWindow.setCentralWidget(editor);

    // Create node palette as dock widget
    auto* palette = new NodePalette(editor);
    auto* dock = new QDockWidget("Node Palette", &mainWindow);
    dock->setWidget(palette);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setStyleSheet(
        "QDockWidget { background: #25252d; color: #ccc; }"
        "QDockWidget::title { background: #333; padding: 4px; }"
    );
    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, dock);

    // Create a simple demo graph
    auto* graph = editor->graph();

    // Add some example nodes
    auto* a = editor->addNode("ConstantInt", 100, 80);
    auto* b = editor->addNode("ConstantInt", 100, 180);
    auto* add = editor->addNode("Add", 350, 120);
    auto* print = editor->addNode("Print", 550, 120);

    // Set constant values
    if (auto* aConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(a))
        aConst->setValue(10);
    if (auto* bConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(b))
        bConst->setValue(20);

    // Create connections
    if (a && add && print) {
        auto* aOut = a->outputs()[0].get();
        auto* addInA = add->inputs()[0].get();
        auto* bOut = b->outputs()[0].get();
        auto* addInB = add->inputs()[1].get();
        auto* addOut = add->outputs()[0].get();
        auto* printIn = print->inputs()[0].get();

        graph->connect(aOut, addInA);
        graph->connect(bOut, addInB);
        graph->connect(addOut, printIn);
    }

    // Rebuild visual scene
    editor->editorScene()->setGraph(graph);
    editor->fitToScreen();

    mainWindow.show();
    return app.exec();
}