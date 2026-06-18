#include <QApplication>
#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QBlock/ThemeManager.h>
#include <QBlock/BuiltinNodes.h>
#include "NodePalette.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QPixmap>
#include <QImage>
#include <QStyleFactory>

/// ============================================================================
/// QCdot-editor - Visual Programming Editor
///
/// A full-featured node-based visual programming editor built on QCdot engine.
/// Features:
///   - Dark/Light theme support
///   - Chinese/English bilingual UI
///   - Node palette with drag-and-drop node creation
///   - Color-coded connection lines by data type
///   - Blank-area node picker popup for quick node creation
///   - Dataflow and Signal dual execution modes
///   - Save/Load to compressed .qcd files
///   - Export graph to .cpp source code
/// ============================================================================

int main(int argc, char* argv[]) {
    qputenv("QSG_RHI_BACKEND", "software");
    QApplication app(argc, argv);

    // Use Fusion style for consistent cross-platform theming
    app.setStyle(QStyleFactory::create("Fusion"));

    // Warmup: Initialize QImage/QPixmap paint engines (MinGW Qt6 workaround)
    { QPixmap _pm(1, 1); QImage _img(1, 1, QImage::Format_ARGB32); (void)_pm; (void)_img; }

    // ---- QCdot-editor application window ----
    QMainWindow mainWindow;
    mainWindow.resize(1280, 800);
    mainWindow.setWindowTitle(QBlock::Translator::tr("app.title"));
    mainWindow.setObjectName(QStringLiteral("QCdotEditor"));

    // Create the node editor as central widget
    auto* editor = new QBlock::NodeEditor(&mainWindow);
    mainWindow.setCentralWidget(editor);

    // Create node palette as dock widget
    auto* palette = new NodePalette(editor);
    auto* dock = new QDockWidget(QBlock::Translator::tr("palette.title"), &mainWindow);
    dock->setWidget(palette);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    // Apply dark theme stylesheet for widgets
    auto applyWindowStyle = [&]() {
        bool isDark = (QBlock::ThemeManager::instance().currentTheme() == QBlock::ThemeMode::Dark);
        if (isDark) {
            mainWindow.setStyleSheet(
                "QMainWindow { background: #1c1c22; }"
                "QDockWidget { background: #25252d; color: #ccc; }"
                "QDockWidget::title { background: #333; padding: 4px; color: #ccc; }"
            );
        } else {
            mainWindow.setStyleSheet(
                "QMainWindow { background: #f0f0f5; }"
                "QDockWidget { background: #fafafc; color: #333; }"
                "QDockWidget::title { background: #e0e0e5; padding: 4px; color: #333; }"
            );
        }
    };
    applyWindowStyle();

    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, dock);

    // Update dock title and main window title on language/theme change
    auto refreshTitles = [&]() {
        mainWindow.setWindowTitle(QBlock::Translator::tr("app.title"));
        dock->setWindowTitle(QBlock::Translator::tr("palette.title"));
    };

    QObject::connect(editor, &QBlock::NodeEditor::languageChanged, &mainWindow, [&]() {
        refreshTitles();
    });

    QBlock::ThemeManager::onThemeChanged([&]() {
        applyWindowStyle();
    });

    // Create a demo graph
    auto* graph = editor->graph();

    auto* a = editor->addNode("ConstantInt", 100, 80);
    auto* b = editor->addNode("ConstantInt", 100, 180);
    auto* add = editor->addNode("Add", 350, 120);
    auto* print = editor->addNode("Print", 550, 120);

    if (auto* aConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(a))
        aConst->setValue(10);
    if (auto* bConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(b))
        bConst->setValue(20);

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

    editor->editorScene()->setGraph(graph);
    editor->fitToScreen();

    mainWindow.show();
    return app.exec();
}