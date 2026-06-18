#include <QBlock/NodeEditor.h>
#include <QBlock/BuiltinNodes.h>
#include <QBlock/Serializer.h>
#include <QBlock/Translator.h>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>

namespace QBlock {

NodeEditor::NodeEditor(QWidget* parent)
    : QWidget(parent)
    , nodeFactory_(Builtin::createBuiltinNode)
{
    setupUI();
    scene_->setGraph(&defaultGraph_);
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

    // Connect signals
    connect(scene_, &EditorScene::graphModified, this, &NodeEditor::graphModified);
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

    // Cache action pointers for language refresh
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

    // Language toggle button
    langAct_ = toolbar_->addAction(Translator::tr("toolbar.lang"));
    connect(langAct_, &QAction::triggered, this, [this]() {
        QString newLang = (Translator::currentLanguage() == QStringLiteral("en"))
                          ? QStringLiteral("zh") : QStringLiteral("en");
        Translator::setLanguage(newLang);
    });

    // Refresh toolbar labels when language changes
    Translator::onLanguageChanged([this, newAct, loadAct, saveAct, execAct, execSigAct, fitAct]() {
        newAct->setText(Translator::tr("toolbar.new"));
        loadAct->setText(Translator::tr("toolbar.open"));
        saveAct->setText(Translator::tr("toolbar.save"));
        execAct->setText(Translator::tr("toolbar.run_dataflow"));
        execSigAct->setText(Translator::tr("toolbar.run_signal"));
        fitAct->setText(Translator::tr("toolbar.fit_view"));
        langAct_->setText(Translator::tr("toolbar.lang"));
        // Refresh node and port widgets to show new language
        for (auto* item : scene_->items()) {
            if (auto* nw = dynamic_cast<NodeWidget*>(item)) {
                nw->update();
            }
        }
        emit languageChanged();
    });

    layout_->addWidget(toolbar_);
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

} // namespace QBlock