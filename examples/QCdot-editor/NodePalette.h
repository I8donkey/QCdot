#ifndef QBLOCK_EXAMPLE_NODE_PALETTE_H
#define QBLOCK_EXAMPLE_NODE_PALETTE_H

#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QListWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDrag>
#include <QMimeData>

class NodePalette : public QWidget {
    Q_OBJECT
public:
    explicit NodePalette(QBlock::NodeEditor* editor, QWidget* parent = nullptr)
        : QWidget(parent), editor_(editor)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);

        // Category filter combo box
        categoryCombo_ = new QComboBox(this);
        categoryCombo_->addItem(tr("All Nodes"), "all");
        categoryCombo_->addItem(tr("Input"), "input");
        categoryCombo_->addItem(tr("Output"), "output");
        categoryCombo_->addItem(tr("Container"), "container");
        categoryCombo_->addItem(tr("Widget"), "widget");
        categoryCombo_->addItem(tr("Widget Method"), "widget_method");
        categoryCombo_->addItem(tr("Math"), "math");
        categoryCombo_->addItem(tr("Logic"), "logic");
        categoryCombo_->addItem(tr("String"), "string");
        categoryCombo_->addItem(tr("Data"), "data");
        categoryCombo_->addItem(tr("Convert"), "convert");
        categoryCombo_->addItem(tr("File"), "file");
        categoryCombo_->addItem(tr("Util"), "util");

        listWidget_ = new QListWidget(this);
        listWidget_->setIconSize(QSize(32, 32));
        listWidget_->setSpacing(8);
        listWidget_->setViewMode(QListView::IconMode);
        listWidget_->setResizeMode(QListView::Adjust);
        listWidget_->setMovement(QListView::Static);
        listWidget_->setGridSize(QSize(72, 88));
        listWidget_->setTextElideMode(Qt::ElideMiddle);
        listWidget_->setDragEnabled(true);
        listWidget_->setDragDropMode(QListWidget::DragOnly);

        layout->addWidget(categoryCombo_);
        layout->addWidget(listWidget_);

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

        connect(categoryCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
            refreshList();
        });

        QBlock::ThemeManager::onThemeChanged([this]() {
            applyThemeStyle();
        });
    }

protected:

private slots:

private:
    void startDrag(Qt::DropActions supportedActions) {
        Q_UNUSED(supportedActions);
        auto* item = listWidget_->currentItem();
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

    struct NodeEntry {
        QString typeName;
        QString trKey;
        QString category;
    };

    void populateNodes() {
        nodes_ = {
            // ===== Input Nodes (输入类) =====
            {"ConstantInt",     "node.ConstantInt",     "input"},
            {"ConstantFloat",   "node.ConstantFloat",   "input"},
            {"ConstantBool",    "node.ConstantBool",    "input"},
            {"ConstantString",  "node.ConstantString",  "input"},
            {"Variable",        "node.Variable",        "input"},
            {"Counter",         "node.Counter",         "input"},
            {"RandomInt",       "node.RandomInt",       "input"},

            // ===== Output Nodes (输出类) =====
            {"Print",           "node.Print",           "output"},

            // ===== Container Nodes (容器类) =====
            {"QtMainWindow",    "node.QtMainWindow",    "container"},
            {"QtTabWidget",     "node.QtTabWidget",     "container"},
            {"QtLayout",        "node.QtLayout",        "container"},

            // ===== Widget Nodes (控件类) =====
            {"QtButton",        "node.QtButton",        "widget"},
            {"QtLabel",         "node.QtLabel",         "widget"},
            {"QtLineEdit",      "node.QtLineEdit",      "widget"},
            {"QtSlider",        "node.QtSlider",        "widget"},
            {"QtCheckBox",      "node.QtCheckBox",      "widget"},
            {"QtComboBox",      "node.QtComboBox",      "widget"},
            {"QtSpinBox",       "node.QtSpinBox",       "widget"},
            {"QtProgressBar",   "node.QtProgressBar",   "widget"},
            {"Color",           "node.Color",           "widget"},

            // ===== Widget Method Nodes (控件方法类) =====
            {"WidgetResize",           "node.WidgetResize",           "widget_method"},
            {"WidgetMove",             "node.WidgetMove",             "widget_method"},
            {"WidgetShow",             "node.WidgetShow",             "widget_method"},
            {"WidgetHide",             "node.WidgetHide",             "widget_method"},
            {"WidgetSetEnabled",       "node.WidgetSetEnabled",       "widget_method"},
            {"WidgetSetVisible",       "node.WidgetSetVisible",       "widget_method"},
            {"WidgetSetStyleSheet",    "node.WidgetSetStyleSheet",    "widget_method"},
            {"WidgetSetToolTip",       "node.WidgetSetToolTip",       "widget_method"},
            {"WidgetSetFixedSize",     "node.WidgetSetFixedSize",     "widget_method"},
            {"WidgetSetMinimumSize",   "node.WidgetSetMinimumSize",   "widget_method"},
            {"WidgetSetMaximumSize",   "node.WidgetSetMaximumSize",   "widget_method"},
            {"WindowSetTitle",         "node.WindowSetTitle",         "widget_method"},
            {"WindowSetGeometry",      "node.WindowSetGeometry",      "widget_method"},
            {"LabelSetText",           "node.LabelSetText",           "widget_method"},
            {"ButtonSetText",          "node.ButtonSetText",          "widget_method"},
            {"LineEditSetText",        "node.LineEditSetText",        "widget_method"},
            {"LineEditGetText",        "node.LineEditGetText",        "widget_method"},
            {"SliderSetValue",         "node.SliderSetValue",         "widget_method"},
            {"SliderGetValue",         "node.SliderGetValue",         "widget_method"},
            {"ProgressBarSetValue",    "node.ProgressBarSetValue",    "widget_method"},
            {"CheckBoxSetChecked",     "node.CheckBoxSetChecked",     "widget_method"},
            {"CheckBoxIsChecked",      "node.CheckBoxIsChecked",      "widget_method"},
            {"SpinBoxSetValue",        "node.SpinBoxSetValue",        "widget_method"},
            {"SpinBoxGetValue",        "node.SpinBoxGetValue",        "widget_method"},
            {"ComboBoxAddItem",        "node.ComboBoxAddItem",        "widget_method"},
            {"ComboBoxGetCurrentText", "node.ComboBoxGetCurrentText", "widget_method"},
            {"WidgetClose",            "node.WidgetClose",            "widget_method"},
            {"WidgetDelete",           "node.WidgetDelete",           "widget_method"},

            // ===== Math Nodes (数学类) =====
            {"Add",             "node.Add",             "math"},
            {"Subtract",        "node.Subtract",        "math"},
            {"Multiply",        "node.Multiply",        "math"},
            {"Divide",          "node.Divide",          "math"},
            {"Modulo",          "node.Modulo",          "math"},
            {"Power",           "node.Power",           "math"},
            {"Sqrt",            "node.Sqrt",            "math"},

            // ===== Logic Nodes (逻辑类) =====
            {"Equal",           "node.Equal",           "logic"},
            {"GreaterThan",     "node.GreaterThan",     "logic"},
            {"LessThan",        "node.LessThan",        "logic"},
            {"And",             "node.And",             "logic"},
            {"Or",              "node.Or",              "logic"},
            {"Not",             "node.Not",             "logic"},
            {"IfElse",          "node.IfElse",          "logic"},
            {"IfElseBranch",    "node.IfElseBranch",    "logic"},

            // ===== String Nodes (字符串类) =====
            {"StringConcat",    "node.StringConcat",    "string"},
            {"StringLength",    "node.StringLength",    "string"},
            {"StringSubstring", "node.StringSubstring", "string"},

            // ===== Data Nodes (数据类) =====
            {"BinaryAnd",       "node.BinaryAnd",       "data"},
            {"BinaryOr",        "node.BinaryOr",        "data"},
            {"BinaryXor",       "node.BinaryXor",       "data"},
            {"ShiftLeft",       "node.ShiftLeft",       "data"},
            {"ShiftRight",      "node.ShiftRight",      "data"},

            // ===== Convert Nodes (转换类) =====
            {"IntToFloat",      "node.IntToFloat",      "convert"},
            {"FloatToInt",      "node.FloatToInt",      "convert"},
            {"ToString",        "node.ToString",        "convert"},

            // ===== File Nodes (文件类) =====
            {"FileReadText",    "node.FileReadText",    "file"},
            {"FileWriteText",   "node.FileWriteText",   "file"},
            {"FileReadBinary",  "node.FileReadBinary",  "file"},

            // ===== Util Nodes (工具类) =====
            {"Sleep",           "node.Sleep",           "util"},
        };
    }

    void refreshList() {
        listWidget_->clear();
        QString currentCategory = categoryCombo_->currentData().toString();

        for (const auto& entry : nodes_) {
            // Filter by category
            if (currentCategory != "all" && entry.category != currentCategory) {
                continue;
            }

            QString display = QBlock::Translator::tr(entry.trKey);
            auto* item = new QListWidgetItem(display, listWidget_);
            item->setIcon(QIcon(QBlock::createNodeIcon(entry.typeName.toStdString())));
            item->setData(Qt::UserRole, entry.typeName);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            item->setToolTip(display);
        }
    }

    void updateTranslations() {
        // Update category combo box translations
        categoryCombo_->setItemText(0, QBlock::Translator::tr("category.all"));
        categoryCombo_->setItemText(1, QBlock::Translator::tr("category.input"));
        categoryCombo_->setItemText(2, QBlock::Translator::tr("category.output"));
        categoryCombo_->setItemText(3, QBlock::Translator::tr("category.container"));
        categoryCombo_->setItemText(4, QBlock::Translator::tr("category.widget"));
        categoryCombo_->setItemText(5, QBlock::Translator::tr("category.widget_method"));
        categoryCombo_->setItemText(6, QBlock::Translator::tr("category.math"));
        categoryCombo_->setItemText(7, QBlock::Translator::tr("category.logic"));
        categoryCombo_->setItemText(8, QBlock::Translator::tr("category.string"));
        categoryCombo_->setItemText(9, QBlock::Translator::tr("category.data"));
        categoryCombo_->setItemText(10, QBlock::Translator::tr("category.convert"));
        categoryCombo_->setItemText(11, QBlock::Translator::tr("category.file"));
        categoryCombo_->setItemText(12, QBlock::Translator::tr("category.util"));
    }

    void applyThemeStyle() {
        QColor bg = QBlock::ThemeManager::panelBg();
        QColor border = QBlock::ThemeManager::panelBorder();
        QColor listBg = QBlock::ThemeManager::listBg();
        QColor itemHover = QBlock::ThemeManager::listItemHover();
        QColor itemSel = QBlock::ThemeManager::listItemSelected();
        QColor text = QBlock::ThemeManager::textPrimary();

        QString style = QString(
            "QComboBox { background: %1; color: %2; border: 1px solid %3; padding: 4px; border-radius: 4px; }"
            "QComboBox::drop-down { border: none; }"
            "QComboBox QAbstractItemView { background: %1; color: %2; selection-background-color: %4; }"
            "QListWidget { background: %1; color: %2; border: 1px solid %3; padding: 6px; }"
            "QListWidget::item { border-radius: 4px; padding: 4px; }"
            "QListWidget::item:selected { background: %4; color: #fff; }"
            "QListWidget::item:hover { background: %5; }"
        ).arg(listBg.name(), text.name(), border.name(), itemSel.name(), itemHover.name());

        setStyleSheet(style);
    }

    QBlock::NodeEditor* editor_;
    QComboBox* categoryCombo_;
    QListWidget* listWidget_;
    std::vector<NodeEntry> nodes_;
};

#endif // QBLOCK_EXAMPLE_NODE_PALETTE_H
