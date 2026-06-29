#ifndef QBLOCK_EXAMPLE_NODE_PALETTE_H
#define QBLOCK_EXAMPLE_NODE_PALETTE_H

#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QDrag>
#include <QMimeData>
#include <QVBoxLayout>

class NodePalette : public QWidget {
    Q_OBJECT
public:
    explicit NodePalette(QBlock::NodeEditor* editor, QWidget* parent = nullptr)
        : QWidget(parent), editor_(editor)
    {
        setObjectName(QStringLiteral("NodePalette"));

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(4);

        searchEdit_ = new QLineEdit(this);
        searchEdit_->setPlaceholderText(QBlock::Translator::tr("palette.search"));
        layout->addWidget(searchEdit_);

        categoryCombo_ = new QComboBox(this);
        layout->addWidget(categoryCombo_);

        list_ = new QListWidget(this);
        list_->setIconSize(QSize(32, 32));
        list_->setSpacing(8);
        list_->setViewMode(QListView::IconMode);
        list_->setResizeMode(QListView::Adjust);
        list_->setMovement(QListView::Static);
        list_->setGridSize(QSize(72, 88));
        list_->setTextElideMode(Qt::ElideMiddle);
        list_->setDragEnabled(true);
        list_->setDragDropMode(QListWidget::DragOnly);
        layout->addWidget(list_, 1);

        populateNodes();
        populateCategories();
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

        connect(searchEdit_, &QLineEdit::textChanged, this, &NodePalette::refreshList);
        connect(categoryCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NodePalette::refreshList);
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
            {"StdVector",       "node.StdVector",       "container"},
            {"StdVectorPush",   "node.StdVectorPush",   "container"},
            {"StdQueue",        "node.StdQueue",        "container"},
            {"StdQueuePush",    "node.StdQueuePush",    "container"},
            {"StdStack",        "node.StdStack",        "container"},
            {"StdStackPush",    "node.StdStackPush",    "container"},
            {"StdDeque",        "node.StdDeque",        "container"},
            {"StdDequePush",    "node.StdDequePush",    "container"},
            {"StdSet",          "node.StdSet",          "container"},
            {"StdSetInsert",    "node.StdSetInsert",    "container"},
            {"StdUnorderedSet", "node.StdUnorderedSet", "container"},
            {"StdUnorderedSetInsert", "node.StdUnorderedSetInsert", "container"},
            {"StdPair",         "node.StdPair",         "container"},
            {"Struct",          "node.Struct",          "container"},
            {"Void",            "node.Void",            "container"},
            {"Sort",            "node.Sort",            "container"},
            {"InputBlock",      "node.InputBlock",      "entry"},
            {"EventBlock",      "node.EventBlock",      "entry"},
        };
    }

    void populateCategories() {
        categoryCombo_->addItem(QBlock::Translator::tr("palette.all"), QStringLiteral("all"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.math"), QStringLiteral("math"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.logic"), QStringLiteral("logic"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.data"), QStringLiteral("data"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.convert"), QStringLiteral("convert"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.string"), QStringLiteral("string"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.output"), QStringLiteral("output"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.bitwise"), QStringLiteral("bitwise"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.file"), QStringLiteral("file"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.util"), QStringLiteral("util"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.qt"), QStringLiteral("qt"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.container"), QStringLiteral("container"));
        categoryCombo_->addItem(QBlock::Translator::tr("palette.entry"), QStringLiteral("entry"));
    }

    void refreshList() {
        list_->clear();

        QString searchText = searchEdit_->text().toLower();
        QString selectedCategory = categoryCombo_->currentData().toString();

        for (const auto& entry : nodes_) {
            if (selectedCategory != QStringLiteral("all") && entry.category != selectedCategory) {
                continue;
            }

            QString display = QBlock::Translator::tr(entry.trKey);
            if (display == entry.trKey) display = entry.typeName;

            bool matchSearch = searchText.isEmpty();
            if (!matchSearch) {
                matchSearch = display.toLower().contains(searchText) ||
                              entry.typeName.toLower().contains(searchText);
            }
            if (!matchSearch) continue;

            auto* item = new QListWidgetItem(display, list_);
            item->setIcon(QIcon(QBlock::createNodeIcon(entry.typeName.toStdString())));
            item->setData(Qt::UserRole, entry.typeName);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            item->setToolTip(display);
        }
    }

    void updateTranslations() {
        setWindowTitle(QBlock::Translator::tr("palette.nodes"));
        searchEdit_->setPlaceholderText(QBlock::Translator::tr("palette.search"));

        int currentIndex = categoryCombo_->currentIndex();
        categoryCombo_->clear();
        populateCategories();
        categoryCombo_->setCurrentIndex(currentIndex);
    }

    void applyThemeStyle() {
        QColor bg = QBlock::ThemeManager::panelBg();
        QColor border = QBlock::ThemeManager::panelBorder();
        QColor listBg = QBlock::ThemeManager::listBg();
        QColor itemHover = QBlock::ThemeManager::listItemHover();
        QColor itemSel = QBlock::ThemeManager::listItemSelected();
        QColor text = QBlock::ThemeManager::textPrimary();

        QString style = QString(
            "QWidget#NodePalette { background: %1; border: 1px solid %2; }"
            "QLineEdit { background: %3; color: %4; border: 1px solid %5; padding: 4px 8px; border-radius: 4px; }"
            "QComboBox { background: %3; color: %4; border: 1px solid %5; padding: 4px 8px; border-radius: 4px; }"
            "QListWidget { background: %3; color: %4; border: 1px solid %5; padding: 6px; }"
            "QListWidget::item { border-radius: 4px; padding: 4px; }"
            "QListWidget::item:selected { background: %6; color: #fff; }"
            "QListWidget::item:hover { background: %7; }"
        ).arg(bg.name(), border.name(), listBg.name(), text.name(),
              border.name(), itemSel.name(), itemHover.name());

        setStyleSheet(style);
    }

    QBlock::NodeEditor* editor_;
    std::vector<NodeEntry> nodes_;
    QLineEdit* searchEdit_ = nullptr;
    QComboBox* categoryCombo_ = nullptr;
    QListWidget* list_ = nullptr;
};

#endif // QBLOCK_EXAMPLE_NODE_PALETTE_H
