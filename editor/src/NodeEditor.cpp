#include <QBlock/NodeEditor.h>
#include <QBlock/BuiltinNodes.h>
#include <QBlock/Serializer.h>
#include <QBlock/Translator.h>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QToolButton>

namespace QBlock {

NodeEditor::NodeEditor(QWidget* parent)
    : QWidget(parent)
    , nodeFactory_(Builtin::createBuiltinNode)
{
    setupUI();
    scene_->setGraph(&defaultGraph_);

    // Register callbacks for blank-area popup
    scene_->setTypeLister([this]() { return listNodeTypes(); });
    scene_->setNodeCreator([this](const std::string& type, float x, float y) {
        return addNode(type, x, y);
    });
}

NodeEditor::~NodeEditor() = default;

void NodeEditor::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    setupToolbar();

    scene_ = new EditorScene(this);
    view_ = new QGraphicsView(scene_, this);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view_->setDragMode(QGraphicsView::ScrollHandDrag);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setFrameShape(QFrame::NoFrame);

    layout_->addWidget(view_);

    connect(scene_, &EditorScene::graphModified, this, &NodeEditor::graphModified);
}

void NodeEditor::setupSettingsMenu(QToolBar* toolbar) {
    // Settings button at the far right of the toolbar
    auto* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    auto* settingsBtn = new QToolButton();
    settingsBtn->setText(QStringLiteral("\u2699")); // gear symbol
    settingsBtn->setToolTip(Translator::tr("toolbar.settings"));
    settingsBtn->setStyleSheet(
        "QToolButton { color: #ccc; background: #444; border: 1px solid #555; "
        "  border-radius: 3px; padding: 4px 10px; margin: 1px; font-size: 16px; }"
        "QToolButton:hover { background: #555; color: #fff; }"
        "QToolButton::menu-indicator { image: none; }"
    );
    settingsBtn->setPopupMode(QToolButton::InstantPopup);

    settingsMenu_ = new QMenu(this);
    settingsMenu_->setStyleSheet(
        "QMenu { background: #333; color: #ccc; border: 1px solid #555; }"
        "QMenu::item { padding: 6px 24px; }"
        "QMenu::item:selected { background: #4a4a5a; }"
        "QMenu::separator { height: 1px; background: #555; margin: 4px 8px; }"
    );

    // Language submenu
    auto* langMenu = settingsMenu_->addMenu(Translator::tr("toolbar.lang"));
    langMenu->setStyleSheet(settingsMenu_->styleSheet());

    auto* enAct = langMenu->addAction(QStringLiteral("English"));
    connect(enAct, &QAction::triggered, this, [this]() {
        setLanguage(QStringLiteral("en"));
    });

    auto* zhAct = langMenu->addAction(QStringLiteral("\u4e2d\u6587")); // 中文
    connect(zhAct, &QAction::triggered, this, [this]() {
        setLanguage(QStringLiteral("zh"));
    });

    settingsMenu_->addSeparator();

    // Theme toggle
    QString themeLabel = (ThemeManager::instance().currentTheme() == ThemeMode::Dark)
        ? Translator::tr("toolbar.theme_light")
        : Translator::tr("toolbar.theme_dark");
    themeAct_ = settingsMenu_->addAction(themeLabel);
    connect(themeAct_, &QAction::triggered, this, &NodeEditor::toggleTheme);

    settingsMenu_->addSeparator();

    // Export to C++ action
    auto* exportAct = settingsMenu_->addAction(Translator::tr("toolbar.export_cpp"));
    connect(exportAct, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getSaveFileName(
            this, Translator::tr("dialog.export_title"), {},
            QStringLiteral("C++ Source (*.cpp)"));
        if (!path.isEmpty()) {
            exportToCpp(path);
        }
    });

    settingsBtn->setMenu(settingsMenu_);
    toolbar->addWidget(settingsBtn);

    // Update settings menu text on language change
    Translator::onLanguageChanged([this]() {
        settingsMenu_->actions()[0]->setText(Translator::tr("toolbar.lang"));
        settingsMenu_->actions()[2]->setText(Translator::tr("toolbar.export_cpp"));
        // Theme label update
        bool isDark = (ThemeManager::instance().currentTheme() == ThemeMode::Dark);
        themeAct_->setText(isDark ? Translator::tr("toolbar.theme_light")
                                   : Translator::tr("toolbar.theme_dark"));
    });
}

void NodeEditor::setupToolbar() {
    toolbar_ = new QToolBar("Tools", this);
    toolbar_->setIconSize(QSize(16, 16));
    toolbar_->setStyleSheet(
        "QToolBar { background: #333; border: none; padding: 2px; spacing: 4px; }"
        "QToolButton { color: #ccc; background: #444; border: 1px solid #555; "
        "  border-radius: 3px; padding: 4px 8px; margin: 1px; }"
        "QToolButton:hover { background: #555; color: #fff; }"
    );

    auto* newAct = toolbar_->addAction(Translator::tr("toolbar.new"));
    connect(newAct, &QAction::triggered, this, &NodeEditor::newGraph);

    auto* loadAct = toolbar_->addAction(Translator::tr("toolbar.open"));
    connect(loadAct, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getOpenFileName(
            this, Translator::tr("dialog.open_title"), {},
            Translator::tr("dialog.filter"));
        if (!path.isEmpty()) loadFromFile(path);
    });

    auto* saveAct = toolbar_->addAction(Translator::tr("toolbar.save"));
    connect(saveAct, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getSaveFileName(
            this, Translator::tr("dialog.save_title"), {},
            Translator::tr("dialog.filter"));
        if (!path.isEmpty()) saveToFile(path);
    });

    toolbar_->addSeparator();

    auto* execAct = toolbar_->addAction(Translator::tr("toolbar.run_dataflow"));
    connect(execAct, &QAction::triggered, this, &NodeEditor::executeDataflow);

    auto* execSigAct = toolbar_->addAction(Translator::tr("toolbar.run_signal"));
    connect(execSigAct, &QAction::triggered, this, [this]() { executeSignal(); });

    toolbar_->addSeparator();

    auto* fitAct = toolbar_->addAction(Translator::tr("toolbar.fit_view"));
    connect(fitAct, &QAction::triggered, this, &NodeEditor::fitToScreen);

    // Settings menu on the right side
    setupSettingsMenu(toolbar_);

    // Refresh toolbar labels when language changes
    Translator::onLanguageChanged([this, newAct, loadAct, saveAct, execAct, execSigAct, fitAct]() {
        newAct->setText(Translator::tr("toolbar.new"));
        loadAct->setText(Translator::tr("toolbar.open"));
        saveAct->setText(Translator::tr("toolbar.save"));
        execAct->setText(Translator::tr("toolbar.run_dataflow"));
        execSigAct->setText(Translator::tr("toolbar.run_signal"));
        fitAct->setText(Translator::tr("toolbar.fit_view"));
        for (auto* item : scene_->items()) {
            if (auto* nw = dynamic_cast<NodeWidget*>(item)) {
                nw->update();
            }
        }
        emit languageChanged();
    });

    layout_->addWidget(toolbar_);
}

void NodeEditor::setLanguage(const QString& lang) {
    Translator::setLanguage(lang);
}

void NodeEditor::newGraph() {
    defaultGraph_.clear();
    scene_->setGraph(&defaultGraph_);
    emit graphModified();
}

bool NodeEditor::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("Error"),
                             Translator::tr("dialog.open_failed") + filePath);
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    auto result = Serializer::fromBytes(data, nodeFactory_);
    if (!result) {
        QMessageBox::warning(this, QStringLiteral("Error"),
                             Translator::tr("dialog.parse_failed"));
        return false;
    }

    *graph() = std::move(*result);
    scene_->setGraph(graph());
    emit graphModified();
    fitToScreen();
    return true;
}

bool NodeEditor::saveToFile(const QString& filePath) const {
    if (!graph()) return false;

    scene_->syncPositionsToGraph();
    QByteArray data = Serializer::toBytes(*graph());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(const_cast<NodeEditor*>(this), QStringLiteral("Error"),
                             Translator::tr("dialog.write_failed") + filePath);
        return false;
    }

    file.write(data);
    file.close();
    return true;
}

bool NodeEditor::exportToCpp(const QString& filePath) const {
    auto* g = graph();
    if (!g || g->empty()) {
        QMessageBox::warning(const_cast<NodeEditor*>(this), QStringLiteral("Error"),
                             Translator::tr("dialog.export_empty"));
        return false;
    }

    scene_->syncPositionsToGraph();
    std::string code = generateCppCode();

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(const_cast<NodeEditor*>(this), QStringLiteral("Error"),
                             Translator::tr("dialog.write_failed") + filePath);
        return false;
    }

    file.write(QByteArray::fromStdString(code));
    file.close();
    return true;
}

std::string NodeEditor::generateCppCode() const {
    auto* g = graph();
    if (!g) return {};

    std::string out;
    out += "// Generated by QCdot-editor\n";
    out += "// Visual Programming Graph Export\n\n";
    out += "#include <cstdint>\n";
    out += "#include <string>\n";
    out += "#include <cmath>\n";
    out += "#include <iostream>\n\n";
    out += "int main() {\n";

    int varCounter = 0;
    for (const auto& node : g->nodes()) {
        std::string varName = "var" + std::to_string(varCounter++);
        std::string typeName = node->typeName();

        if (typeName == "ConstantInt") {
            out += "    int " + varName + " = 0;\n";
        } else if (typeName == "ConstantFloat") {
            out += "    double " + varName + " = 0.0;\n";
        } else if (typeName == "ConstantBool") {
            out += "    bool " + varName + " = false;\n";
        } else if (typeName == "ConstantString") {
            out += "    std::string " + varName + " = \"\";\n";
        } else if (typeName == "Print") {
            out += "    std::cout << " + varName + " << std::endl;\n";
        } else if (typeName == "Add" || typeName == "Subtract" ||
                   typeName == "Multiply" || typeName == "Divide") {
            out += "    // " + typeName + " operation\n";
            out += "    auto " + varName + " = 0;\n";
        } else {
            out += "    // Node: " + typeName + " (variable " + varName + ")\n";
        }
    }
    out += "    return 0;\n";
    out += "}\n";
    return out;
}

void NodeEditor::executeDataflow() {
    auto* g = graph();
    if (!g || g->empty()) return;
    scene_->syncPositionsToGraph();
    engine_.setMode(ExecutionMode::Dataflow);
    engine_.executeDataflow(*g);
}

void NodeEditor::executeSignal(Node* entryNode) {
    auto* g = graph();
    if (!g || g->empty()) return;
    scene_->syncPositionsToGraph();
    engine_.setMode(ExecutionMode::Signal);

    if (!entryNode && !g->nodes().empty()) {
        entryNode = g->nodes().front().get();
    }

    if (entryNode) {
        engine_.executeSignal(*g, entryNode);
    }
}

std::vector<std::string> NodeEditor::listNodeTypes() const {
    // Known builtin node types
    return {
        "Add", "Subtract", "Multiply", "Divide", "Modulo", "Power", "Sqrt",
        "Equal", "GreaterThan", "LessThan", "And", "Or", "Not",
        "IfElse", "IfElseBranch",
        "ConstantInt", "ConstantFloat", "ConstantBool", "ConstantString",
        "IntToFloat", "FloatToInt", "ToString",
        "StringConcat", "StringLength", "StringSubstring",
        "Print",
        "BinaryAnd", "BinaryOr", "BinaryXor", "ShiftLeft", "ShiftRight",
        "FileReadText", "FileWriteText", "FileReadBinary",
        "RandomInt", "Sleep"
    };
}

Node* NodeEditor::addNode(const std::string& typeName, float x, float y) {
    auto* g = graph();
    if (!g || !nodeFactory_) return nullptr;

    auto node = nodeFactory_(typeName);
    if (!node) return nullptr;

    node->setPosition(x, y);
    auto* ptr = g->addNode(std::move(node));
    scene_->addNodeWidget(ptr);
    emit graphModified();
    return ptr;
}

void NodeEditor::fitToScreen() {
    view_->fitInView(scene_->itemsBoundingRect().adjusted(-50, -50, 50, 50),
                     Qt::KeepAspectRatio);
}

void NodeEditor::toggleTheme() {
    ThemeMode current = ThemeManager::instance().currentTheme();
    ThemeMode next = (current == ThemeMode::Dark) ? ThemeMode::Light : ThemeMode::Dark;
    ThemeManager::instance().setTheme(next);
    applyThemeStyle();

    // Update theme action text
    QString themeLabel = (next == ThemeMode::Dark)
        ? Translator::tr("toolbar.theme_light")
        : Translator::tr("toolbar.theme_dark");
    if (themeAct_) themeAct_->setText(themeLabel);
}

void NodeEditor::applyThemeStyle() {
    bool isDark = (ThemeManager::instance().currentTheme() == ThemeMode::Dark);

    if (isDark) {
        toolbar_->setStyleSheet(
            "QToolBar { background: #333; border: none; padding: 2px; spacing: 4px; }"
            "QToolButton { color: #ccc; background: #444; border: 1px solid #555; "
            "  border-radius: 3px; padding: 4px 8px; margin: 1px; }"
            "QToolButton:hover { background: #555; color: #fff; }"
        );
    } else {
        toolbar_->setStyleSheet(
            "QToolBar { background: #e6e6eb; border: none; padding: 2px; spacing: 4px; }"
            "QToolButton { color: #333; background: #d7d7e1; border: 1px solid #c8c8d2; "
            "  border-radius: 3px; padding: 4px 8px; margin: 1px; }"
            "QToolButton:hover { background: #c8c8d7; color: #000; }"
        );
    }

    // Refresh the scene background
    scene_->setBackgroundBrush(ThemeManager::background());
    scene_->update();
}

} // namespace QBlock