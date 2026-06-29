#include <QApplication>
#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QBlock/ThemeManager.h>
#include <QBlock/BuiltinNodes.h>
#include "NodePalette.h"
#include "ConsoleWidget.h"
#include "ConsoleRedirector.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QPixmap>
#include <QImage>
#include <QStyleFactory>
#include <QShortcut>
#include <QFileDialog>

int main(int argc, char* argv[]) {
    qputenv("QSG_RHI_BACKEND", "software");
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    { QPixmap _pm(1, 1); QImage _img(1, 1, QImage::Format_ARGB32); (void)_pm; (void)_img; }

    QMainWindow mainWindow;
    mainWindow.resize(1280, 800);
    mainWindow.setWindowTitle(QBlock::Translator::tr("app.title"));
    mainWindow.setObjectName(QStringLiteral("QCdotEditor"));

    auto* editor = new QBlock::NodeEditor(&mainWindow);
    mainWindow.setCentralWidget(editor);

    auto* palette = new NodePalette(editor);
    auto* dock = new QDockWidget(QBlock::Translator::tr("palette.title"), &mainWindow);
    dock->setWidget(palette);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    auto* consoleWidget = new ConsoleWidget();
    auto* consoleDock = new QDockWidget(QBlock::Translator::tr("toolbar.console"), &mainWindow);
    consoleDock->setWidget(consoleWidget);
    consoleDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    
    QObject::connect(editor, &QBlock::NodeEditor::toggleConsoleRequested, &mainWindow, [consoleDock](bool checked) {
        consoleDock->setVisible(checked);
    });
    QObject::connect(consoleDock, &QDockWidget::visibilityChanged, editor, [editor](bool visible) {
        for (QAction* act : editor->findChildren<QAction*>()) {
            if (act->text() == QBlock::Translator::tr("toolbar.console")) {
                act->setChecked(visible);
                break;
            }
        }
    });

    auto applyWindowStyle = [&]() {
        bool isDark = (QBlock::ThemeManager::instance().currentTheme() == QBlock::ThemeMode::Dark);
        consoleWidget->setDarkMode(isDark);
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
    mainWindow.addDockWidget(Qt::BottomDockWidgetArea, consoleDock);

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

    auto* redirector = new ConsoleRedirector(&mainWindow);
    QObject::connect(redirector, &ConsoleRedirector::outputReceived, consoleWidget, &ConsoleWidget::appendText);

    auto* graph = editor->graph();

    auto* intA = editor->addNode("ConstantInt", 80, 60);
    auto* intB = editor->addNode("ConstantInt", 80, 160);
    if (auto* aConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(intA))
        aConst->setValue(10);
    if (auto* bConst = dynamic_cast<QBlock::Builtin::ConstantIntNode*>(intB))
        bConst->setValue(20);

    auto* floatA = editor->addNode("ConstantFloat", 80, 260);
    if (auto* fConst = dynamic_cast<QBlock::Builtin::ConstantFloatNode*>(floatA))
        fConst->setValue(3.14159);

    auto* boolA = editor->addNode("ConstantBool", 80, 360);
    if (auto* bConst = dynamic_cast<QBlock::Builtin::ConstantBoolNode*>(boolA))
        bConst->setValue(true);

    auto* strA = editor->addNode("ConstantString", 80, 460);
    if (auto* sConst = dynamic_cast<QBlock::Builtin::ConstantStringNode*>(strA))
        sConst->setValue("Hello, qcdot!");

    auto* add = editor->addNode("Add", 320, 110);
    auto* print = editor->addNode("Print", 560, 150);

    if (intA && add) graph->connect(intA->outputs()[0].get(), add->inputs()[0].get());
    if (intB && add) graph->connect(intB->outputs()[0].get(), add->inputs()[1].get());
    if (add && print)  graph->connect(add->outputs()[0].get(), print->inputs()[0].get());

    editor->editorScene()->setGraph(graph);
    editor->fitToScreen();

    auto* shortcutUndo = new QShortcut(QKeySequence("Ctrl+Z"), &mainWindow);
    QObject::connect(shortcutUndo, &QShortcut::activated, &mainWindow, [&]() {
        editor->undoAction();
    });

    auto* shortcutRedo = new QShortcut(QKeySequence("Ctrl+Y"), &mainWindow);
    QObject::connect(shortcutRedo, &QShortcut::activated, &mainWindow, [&]() {
        editor->redoAction();
    });

    auto* shortcutSelectAll = new QShortcut(QKeySequence("Ctrl+A"), &mainWindow);
    QObject::connect(shortcutSelectAll, &QShortcut::activated, &mainWindow, [&]() {
        editor->selectAllNodes();
    });

    auto* shortcutNew = new QShortcut(QKeySequence("Ctrl+N"), &mainWindow);
    QObject::connect(shortcutNew, &QShortcut::activated, &mainWindow, [&]() {
        editor->newGraph();
    });

    auto* shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), &mainWindow);
    QObject::connect(shortcutOpen, &QShortcut::activated, &mainWindow, [&]() {
        QString path = QFileDialog::getOpenFileName(
            &mainWindow, QBlock::Translator::tr("dialog.open_title"), {},
            QBlock::Translator::tr("dialog.filter"));
        if (!path.isEmpty()) {
            editor->loadFromFile(path);
        }
    });

    auto* shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), &mainWindow);
    QObject::connect(shortcutSave, &QShortcut::activated, &mainWindow, [&]() {
        QString path = QFileDialog::getSaveFileName(
            &mainWindow, QBlock::Translator::tr("dialog.save_title"), {},
            QBlock::Translator::tr("dialog.filter"));
        if (!path.isEmpty()) {
            editor->saveToFile(path);
        }
    });

    mainWindow.show();
    return app.exec();
}
