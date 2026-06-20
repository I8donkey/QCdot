#ifndef QBLOCK_EXAMPLE_NODE_PALETTE_H
#define QBLOCK_EXAMPLE_NODE_PALETTE_H

#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QListWidget>
#include <QComboBox>
#include <QDrag>
#include <QMimeData>

class NodePalette : public QListWidget {
    Q_OBJECT
public:
    explicit NodePalette(QBlock::NodeEditor* editor, QWidget* parent = nullptr)
        : QListWidget(parent), editor_(editor)
    {
        setIconSize(QSize(32, 32));
        setSpacing(8);
        setViewMode(QListView::IconMode);
        setResizeMode(QListView::Adjust);
        setMovement(QListView::Static);
        setGridSize(QSize(72, 88));
        setTextElideMode(Qt::ElideMiddle);
        setDragEnabled(true);
        setDragDropMode(QListWidget::DragOnly);

        populateNodes();
        refreshList();
        updateTranslations();
        applyThemeStyle();

        if (editor_) {
            connect(editor_, &QBlock::NodeEditor::languageChanged, this, [this]() {
                refreshList();
                updateTranslations();
            });
        }

        QBlock::ThemeManager::onThemeChanged([this]() {
            applyThemeStyle();
        });
    }

protected:
    void startDrag(Qt::DropActions supportedActions) override {
        Q_UNUSED(supportedActions);
        auto* item = currentItem();
        if (!item) return;

        auto* mimeData = new QMimeData();
        QString typeName = item->data(Qt::UserRole).toString();
        mimeData->setData("application/x-qblock-node-type", typeName.toUtf8());

        auto* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(item->icon().pixmap(32, 32));
        drag->setHotSpot(QPoint(16, 16));

        drag->exec(Qt::CopyAction);
    }

private:
    struct NodeEntry {
        QString typeName;
        QString trKey;
        QString category;
    };

    void populateNodes() {
        nodes_ = {
            {"Add",             "node.Add",             "math"},
            {"Subtract",        "node.Subtract",        "math"},
            {"Multiply",        "node.Multiply",        "math"},
            {"Divide",          "node.Divide",          "math"},
            {"Modulo",          "node.Modulo",          "math"},
            {"Power",           "node.Power",           "math"},
            {"Sqrt",            "node.Sqrt",            "math"},
            {"Equal",           "node.Equal",           "logic"},
            {"GreaterThan",     "node.GreaterThan",     "logic"},
            {"LessThan",        "node.LessThan",        "logic"},
            {"And",             "node.And",             "logic"},
            {"Or",              "node.Or",              "logic"},
            {"Not",             "node.Not",             "logic"},
            {"IfElse",          "node.IfElse",          "logic"},
            {"IfElseBranch",    "node.IfElseBranch",    "logic"},
            {"ConstantInt",     "node.ConstantInt",     "data"},
            {"ConstantFloat",   "node.ConstantFloat",   "data"},
            {"ConstantBool",    "node.ConstantBool",    "data"},
            {"ConstantString",  "node.ConstantString",  "data"},
            {"Variable",        "node.Variable",        "data"},
            {"Counter",         "node.Counter",         "data"},
            {"IntToFloat",      "node.IntToFloat",      "convert"},
            {"FloatToInt",      "node.FloatToInt",      "convert"},
            {"ToString",        "node.ToString",        "convert"},
            {"StringConcat",    "node.StringConcat",    "string"},
            {"StringLength",    "node.StringLength",    "string"},
            {"StringSubstring", "node.StringSubstring", "string"},
            {"Print",           "node.Print",           "output"},
            {"BinaryAnd",       "node.BinaryAnd",       "bitwise"},
            {"BinaryOr",        "node.BinaryOr",        "bitwise"},
            {"BinaryXor",       "node.BinaryXor",       "bitwise"},
            {"ShiftLeft",       "node.ShiftLeft",       "bitwise"},
            {"ShiftRight",      "node.ShiftRight",      "bitwise"},
            {"FileReadText",    "node.FileReadText",    "file"},
            {"FileWriteText",   "node.FileWriteText",   "file"},
            {"FileReadBinary",  "node.FileReadBinary",  "file"},
            {"RandomInt",       "node.RandomInt",       "util"},
            {"Sleep",           "node.Sleep",           "util"},
            {"Color",           "node.Color",           "qt"},
            {"QtMainWindow",    "node.QtMainWindow",    "qt"},
            {"QtButton",        "node.QtButton",        "qt"},
            {"QtLabel",         "node.QtLabel",         "qt"},
            {"QtLineEdit",      "node.QtLineEdit",      "qt"},
            {"QtTabWidget",     "node.QtTabWidget",     "qt"},
            {"QtLayout",        "node.QtLayout",        "qt"},
            {"QtSlider",        "node.QtSlider",        "qt"},
            {"QtCheckBox",      "node.QtCheckBox",      "qt"},
            {"QtComboBox",      "node.QtComboBox",      "qt"},
            {"QtSpinBox",       "node.QtSpinBox",       "qt"},
            {"QtProgressBar",   "node.QtProgressBar",   "qt"},
        };
    }

    void refreshList() {
        clear();

        for (const auto& entry : nodes_) {
            QString display = QBlock::Translator::tr(entry.trKey);
            auto* item = new QListWidgetItem(display, this);
            item->setIcon(QIcon(QBlock::createNodeIcon(entry.typeName.toStdString())));
            item->setData(Qt::UserRole, entry.typeName);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            item->setToolTip(display);
        }
    }

    void updateTranslations() {
        setWindowTitle(QBlock::Translator::tr("palette.nodes"));
    }

    void applyThemeStyle() {
        QColor bg = QBlock::ThemeManager::panelBg();
        QColor border = QBlock::ThemeManager::panelBorder();
        QColor listBg = QBlock::ThemeManager::listBg();
        QColor itemHover = QBlock::ThemeManager::listItemHover();
        QColor itemSel = QBlock::ThemeManager::listItemSelected();
        QColor text = QBlock::ThemeManager::textPrimary();

        QString style = QString(
            "QListWidget { background: %1; color: %2; border: 1px solid %3; padding: 6px; }"
            "QListWidget::item { border-radius: 4px; padding: 4px; }"
            "QListWidget::item:selected { background: %4; color: #fff; }"
            "QListWidget::item:hover { background: %5; }"
        ).arg(listBg.name(), text.name(), border.name(), itemSel.name(), itemHover.name());

        setStyleSheet(style);
    }

    QBlock::NodeEditor* editor_;
    std::vector<NodeEntry> nodes_;
};

#endif // QBLOCK_EXAMPLE_NODE_PALETTE_H