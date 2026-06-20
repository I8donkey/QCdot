#include <QBlock/NodeEditor.h>
#include <QBlock/BuiltinNodes.h>
#include <QBlock/Serializer.h>
#include <QBlock/Translator.h>
#include <QBlock/Port.h>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QToolButton>
#include <QStandardPaths>
#include <QDir>
#include <QClipboard>
#include <QMimeData>

namespace QBlock {

namespace {
    QString settingsPath() {
        return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
               + QStringLiteral("/qcdot_settings.json");
    }
}

void saveSettings() {
    QString path = settingsPath();
    QDir().mkpath(QFileInfo(path).absolutePath());
    
    QJsonObject obj;
    obj["language"] = Translator::currentLanguage();
    obj["theme"] = (ThemeManager::instance().currentTheme() == ThemeMode::Dark) ? QStringLiteral("dark") : QStringLiteral("light");
    
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson());
    }
}

void loadSettings() {
    QString path = settingsPath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    
    if (obj.contains("language")) {
        Translator::setLanguage(obj["language"].toString());
    }
    if (obj.contains("theme")) {
        QString theme = obj["theme"].toString();
        if (theme == QStringLiteral("dark")) {
            ThemeManager::instance().setTheme(ThemeMode::Dark);
        } else {
            ThemeManager::instance().setTheme(ThemeMode::Light);
        }
    }
}

NodeEditor::NodeEditor(QWidget* parent)
    : QWidget(parent)
    , nodeFactory_(Builtin::createBuiltinNode)
{
    loadSettings();
    setupUI();
    applyThemeStyle();
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

    toolbar_->addSeparator();

    auto* copyAct = toolbar_->addAction(Translator::tr("toolbar.copy"));
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, this, &NodeEditor::copyNodes);

    auto* pasteAct = toolbar_->addAction(Translator::tr("toolbar.paste"));
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, this, &NodeEditor::pasteNodes);

    // Settings menu on the right side
    setupSettingsMenu(toolbar_);

    // Refresh toolbar labels when language changes
    Translator::onLanguageChanged([this, newAct, loadAct, saveAct, execAct, execSigAct, fitAct, copyAct, pasteAct]() {
        newAct->setText(Translator::tr("toolbar.new"));
        loadAct->setText(Translator::tr("toolbar.open"));
        saveAct->setText(Translator::tr("toolbar.save"));
        execAct->setText(Translator::tr("toolbar.run_dataflow"));
        execSigAct->setText(Translator::tr("toolbar.run_signal"));
        fitAct->setText(Translator::tr("toolbar.fit_view"));
        copyAct->setText(Translator::tr("toolbar.copy"));
        pasteAct->setText(Translator::tr("toolbar.paste"));
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
    saveSettings();
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
    out += "#include <iostream>\n";
    out += "#include <QApplication>\n";
    out += "#include <QMainWindow>\n";
    out += "#include <QPushButton>\n";
    out += "#include <QLabel>\n";
    out += "#include <QLineEdit>\n";
    out += "#include <QTabWidget>\n";
    out += "#include <QVBoxLayout>\n";
    out += "#include <QHBoxLayout>\n";
    out += "#include <QComboBox>\n";
    out += "#include <QSlider>\n";
    out += "#include <QCheckBox>\n";
    out += "#include <QSpinBox>\n";
    out += "#include <QProgressBar>\n\n";
    out += "int main(int argc, char* argv[]) {\n";
    out += "    QApplication app(argc, argv);\n\n";

    std::map<const Node*, std::string> nodeVarMap;
    std::map<const Node*, std::string> nodeTypeMap;
    int varCounter = 0;
    for (const auto& node : g->nodes()) {
        std::string varName = "var" + std::to_string(varCounter++);
        nodeVarMap[node.get()] = varName;
        nodeTypeMap[node.get()] = node->typeName();
    }

    std::map<Port*, std::string> portToVarMap;
    for (const auto& conn : g->connections()) {
        auto* srcPort = conn->source();
        auto* tgtPort = conn->target();
        auto* srcNode = srcPort->parentNode();
        if (srcNode) {
            portToVarMap[tgtPort] = nodeVarMap[srcNode];
        }
    }

    std::set<std::string> usedVars;
    std::vector<std::string> declarations;
    std::vector<std::string> statements;
    std::vector<std::string> qtStatements;

    for (const auto& node : g->nodes()) {
        std::string varName = nodeVarMap[node.get()];
        std::string typeName = node->typeName();

        if (typeName == "ConstantInt") {
            std::string value = "0";
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    value = it->second;
                    usedVars.insert(it->second);
                    break;
                }
            }
            declarations.push_back("    int64_t " + varName + " = " + value + ";");
            usedVars.insert(varName);
        } else if (typeName == "ConstantFloat") {
            std::string value = "0.0";
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    value = it->second;
                    usedVars.insert(it->second);
                    break;
                }
            }
            declarations.push_back("    double " + varName + " = " + value + ";");
            usedVars.insert(varName);
        } else if (typeName == "ConstantBool") {
            std::string value = "false";
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    value = it->second;
                    usedVars.insert(it->second);
                    break;
                }
            }
            declarations.push_back("    bool " + varName + " = " + value + ";");
            usedVars.insert(varName);
        } else if (typeName == "ConstantString") {
            std::string value = "\"\"";
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    value = it->second;
                    usedVars.insert(it->second);
                    break;
                }
            }
            declarations.push_back("    std::string " + varName + " = " + value + ";");
            usedVars.insert(varName);
        } else if (typeName == "Print") {
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    statements.push_back("    std::cout << " + it->second + " << std::endl;");
                    usedVars.insert(it->second);
                    break;
                }
            }
        } else if (typeName == "Add") {
            std::string aVar, bVar;
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "a") aVar = it->second;
                    else if (input->name() == "b") bVar = it->second;
                }
            }
            if (!aVar.empty() && !bVar.empty()) {
                declarations.push_back("    double " + varName + " = " + aVar + " + " + bVar + ";");
                usedVars.insert(aVar);
                usedVars.insert(bVar);
                usedVars.insert(varName);
            }
        } else if (typeName == "Subtract") {
            std::string aVar, bVar;
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "a") aVar = it->second;
                    else if (input->name() == "b") bVar = it->second;
                }
            }
            if (!aVar.empty() && !bVar.empty()) {
                declarations.push_back("    double " + varName + " = " + aVar + " - " + bVar + ";");
                usedVars.insert(aVar);
                usedVars.insert(bVar);
                usedVars.insert(varName);
            }
        } else if (typeName == "Multiply") {
            std::string aVar, bVar;
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "a") aVar = it->second;
                    else if (input->name() == "b") bVar = it->second;
                }
            }
            if (!aVar.empty() && !bVar.empty()) {
                declarations.push_back("    double " + varName + " = " + aVar + " * " + bVar + ";");
                usedVars.insert(aVar);
                usedVars.insert(bVar);
                usedVars.insert(varName);
            }
        } else if (typeName == "Divide") {
            std::string aVar, bVar;
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "a") aVar = it->second;
                    else if (input->name() == "b") bVar = it->second;
                }
            }
            if (!aVar.empty() && !bVar.empty()) {
                declarations.push_back("    double " + varName + " = (" + bVar + " != 0) ? (" + aVar + " / " + bVar + ") : 0;");
                usedVars.insert(aVar);
                usedVars.insert(bVar);
                usedVars.insert(varName);
            }
        } else if (typeName == "QtMainWindow") {
            qtStatements.push_back("    QMainWindow* " + varName + " = new QMainWindow();");
            qtStatements.push_back("    " + varName + "->setWindowTitle(\"QCdot Window\");");
            qtStatements.push_back("    " + varName + "->resize(800, 600);");

            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "title") {
                        qtStatements.push_back("    " + varName + "->setWindowTitle(QString::fromStdString(" + it->second + "));");
                        usedVars.insert(it->second);
                    } else if (input->name() == "widget1" || input->name() == "widget2" || input->name() == "widget3") {
                        qtStatements.push_back("    QWidget* centralWidget = new QWidget(" + varName + ");");
                        qtStatements.push_back("    QVBoxLayout* layout = new QVBoxLayout(centralWidget);");
                        qtStatements.push_back("    layout->addWidget(" + it->second + ");");
                        qtStatements.push_back("    " + varName + "->setCentralWidget(centralWidget);");
                        usedVars.insert(it->second);
                    }
                }
            }
            qtStatements.push_back("    " + varName + "->show();");
            usedVars.insert(varName);
        } else if (typeName == "QtButton") {
            qtStatements.push_back("    QPushButton* " + varName + " = new QPushButton(\"Button\");");
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "text") {
                        qtStatements.push_back("    " + varName + "->setText(QString::fromStdString(" + it->second + "));");
                        usedVars.insert(it->second);
                    }
                }
            }
            usedVars.insert(varName);
        } else if (typeName == "QtLabel") {
            qtStatements.push_back("    QLabel* " + varName + " = new QLabel(\"Label\");");
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "text") {
                        qtStatements.push_back("    " + varName + "->setText(QString::fromStdString(" + it->second + "));");
                        usedVars.insert(it->second);
                    }
                }
            }
            usedVars.insert(varName);
        } else if (typeName == "QtLineEdit") {
            qtStatements.push_back("    QLineEdit* " + varName + " = new QLineEdit();");
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "placeholder") {
                        qtStatements.push_back("    " + varName + "->setPlaceholderText(QString::fromStdString(" + it->second + "));");
                        usedVars.insert(it->second);
                    }
                }
            }
            usedVars.insert(varName);
        } else if (typeName == "QtComboBox") {
            qtStatements.push_back("    QComboBox* " + varName + " = new QComboBox();");
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "items") {
                        qtStatements.push_back("    " + varName + "->addItems(QString::fromStdString(" + it->second + ").split(\",\"));");
                        usedVars.insert(it->second);
                    }
                }
            }
            usedVars.insert(varName);
        } else if (typeName == "QtLayout") {
            qtStatements.push_back("    QVBoxLayout* " + varName + " = new QVBoxLayout();");
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "item1" || input->name() == "item2" || input->name() == "item3") {
                        qtStatements.push_back("    " + varName + "->addWidget(" + it->second + ");");
                        usedVars.insert(it->second);
                    }
                }
            }
            usedVars.insert(varName);
        } else if (typeName == "Variable") {
            declarations.push_back("    double " + varName + " = 0.0;");
            usedVars.insert(varName);
        } else if (typeName == "Counter") {
            declarations.push_back("    int64_t " + varName + " = 0;");
            usedVars.insert(varName);
        } else if (typeName == "Modulo") {
            std::string aVar, bVar;
            for (const auto& input : node->inputs()) {
                auto it = portToVarMap.find(input.get());
                if (it != portToVarMap.end()) {
                    if (input->name() == "a") aVar = it->second;
                    else if (input->name() == "b") bVar = it->second;
                }
            }
            if (!aVar.empty() && !bVar.empty()) {
                declarations.push_back("    int64_t " + varName + " = (" + bVar + " != 0) ? (" + aVar + " % " + bVar + ") : 0;");
                usedVars.insert(aVar);
                usedVars.insert(bVar);
                usedVars.insert(varName);
            }
        } else if (typeName == "Power" || typeName == "Sqrt") {
            declarations.push_back("    double " + varName + " = 0.0;");
            usedVars.insert(varName);
        } else if (typeName == "Equal" || typeName == "GreaterThan" || typeName == "LessThan" ||
                   typeName == "And" || typeName == "Or" || typeName == "Not") {
            declarations.push_back("    bool " + varName + " = false;");
            usedVars.insert(varName);
        }
    }

    for (const auto& decl : declarations) {
        out += decl + "\n";
    }

    if (!statements.empty()) {
        out += "\n    // Computations\n";
        for (const auto& stmt : statements) {
            out += stmt + "\n";
        }
    }

    if (!qtStatements.empty()) {
        out += "\n    // Qt UI setup\n";
        for (const auto& stmt : qtStatements) {
            out += stmt + "\n";
        }
    }

    out += "\n    return app.exec();\n";
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
        "Variable", "Counter",
        "IntToFloat", "FloatToInt", "ToString",
        "StringConcat", "StringLength", "StringSubstring",
        "Print",
        "BinaryAnd", "BinaryOr", "BinaryXor", "ShiftLeft", "ShiftRight",
        "FileReadText", "FileWriteText", "FileReadBinary",
        "RandomInt", "Sleep",
        "Variable", "Counter",
        // Qt UI nodes
        "Color", "QtMainWindow", "QtButton", "QtLabel", "QtLineEdit",
        "QtTabWidget", "QtLayout", "QtSlider", "QtCheckBox",
        "QtComboBox", "QtSpinBox", "QtProgressBar"
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

    saveSettings();
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

void NodeEditor::copyNodes() {
    auto selected = scene_->selectedItems();
    if (selected.empty()) return;

    std::vector<std::pair<std::string, QPointF>> copiedNodes;
    QPointF minPos(1e9, 1e9);

    for (auto* item : selected) {
        if (auto* nw = dynamic_cast<NodeWidget*>(item)) {
            copiedNodes.emplace_back(nw->node()->typeName(), nw->pos());
            if (nw->pos().x() < minPos.x()) minPos.setX(nw->pos().x());
            if (nw->pos().y() < minPos.y()) minPos.setY(nw->pos().y());
        }
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint32>(copiedNodes.size());
    for (const auto& [type, pos] : copiedNodes) {
        stream << QString::fromStdString(type);
        stream << pos;
    }

    QClipboard* clipboard = QApplication::clipboard();
    QMimeData* mimeData = new QMimeData();
    mimeData->setData("application/x-qblock-nodes", data);
    clipboard->setMimeData(mimeData);
}

void NodeEditor::pasteNodes() {
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    if (!mimeData || !mimeData->hasFormat("application/x-qblock-nodes")) return;

    QByteArray data = mimeData->data("application/x-qblock-nodes");
    QDataStream stream(&data, QIODevice::ReadOnly);

    quint32 count = 0;
    stream >> count;

    std::vector<std::pair<std::string, QPointF>> nodesToPaste;
    QPointF minPos(1e9, 1e9);

    for (quint32 i = 0; i < count; ++i) {
        QString type;
        QPointF pos;
        stream >> type >> pos;
        nodesToPaste.emplace_back(type.toStdString(), pos);
        if (pos.x() < minPos.x()) minPos.setX(pos.x());
        if (pos.y() < minPos.y()) minPos.setY(pos.y());
    }

    QPointF offset(50, 50);
    for (const auto& [type, pos] : nodesToPaste) {
        QPointF newPos = pos - minPos + offset;
        addNode(type, static_cast<float>(newPos.x()), static_cast<float>(newPos.y()));
    }
}

} // namespace QBlock