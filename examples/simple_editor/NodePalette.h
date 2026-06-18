#ifndef QBLOCK_EXAMPLE_NODE_PALETTE_H
#define QBLOCK_EXAMPLE_NODE_PALETTE_H

#include <QBlock/NodeEditor.h>
#include <QBlock/Translator.h>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>

/// Side panel with a list of available nodes for drag-and-drop.
/// Full Chinese/English bilingual support.
class NodePalette : public QWidget {
    Q_OBJECT
public:
    explicit NodePalette(QBlock::NodeEditor* editor, QWidget* parent = nullptr)
        : QWidget(parent), editor_(editor)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);

        titleLabel_ = new QLabel();
        titleLabel_->setStyleSheet("color: #ccc; padding: 4px;");
        titleLabel_->setTextFormat(Qt::RichText);
        layout->addWidget(titleLabel_);

        list_ = new QListWidget(this);
        list_->setStyleSheet(
            "QListWidget { background: #2a2a32; border: 1px solid #444; color: #ccc; }"
            "QListWidget::item { padding: 4px 8px; }"
            "QListWidget::item:hover { background: #3a3a48; }"
            "QListWidget::item:selected { background: #4a4a5a; }"
        );
        layout->addWidget(list_);

        populateNodes();
        updateTranslations();

        connect(list_, &QListWidget::itemDoubleClicked, this, &NodePalette::onNodeDoubleClicked);

        if (editor_) {
            connect(editor_, &QBlock::NodeEditor::languageChanged, this, [this]() {
                refreshList();
                updateTranslations();
            });
        }
    }

private:
    struct NodeEntry {
        QString typeName;  // internal type name (not translated)
        QString trKey;     // translation key
    };

    void populateNodes() {
        // Same order as the original list
        nodes_ = {
            {"Add",            QStringLiteral("node.Add")},
            {"Subtract",       QStringLiteral("node.Subtract")},
            {"Multiply",       QStringLiteral("node.Multiply")},
            {"Divide",         QStringLiteral("node.Divide")},
            {"Modulo",         QStringLiteral("node.Modulo")},
            {"Power",          QStringLiteral("node.Power")},
            {"Sqrt",           QStringLiteral("node.Sqrt")},
            {"Equal",          QStringLiteral("node.Equal")},
            {"GreaterThan",    QStringLiteral("node.GreaterThan")},
            {"LessThan",       QStringLiteral("node.LessThan")},
            {"And",            QStringLiteral("node.And")},
            {"Or",             QStringLiteral("node.Or")},
            {"Not",            QStringLiteral("node.Not")},
            {"IfElse",         QStringLiteral("node.IfElse")},
            {"IfElseBranch",   QStringLiteral("node.IfElseBranch")},
            {"ConstantInt",    QStringLiteral("node.ConstantInt")},
            {"ConstantFloat",  QStringLiteral("node.ConstantFloat")},
            {"ConstantBool",   QStringLiteral("node.ConstantBool")},
            {"ConstantString", QStringLiteral("node.ConstantString")},
            {"IntToFloat",     QStringLiteral("node.IntToFloat")},
            {"FloatToInt",     QStringLiteral("node.FloatToInt")},
            {"ToString",       QStringLiteral("node.ToString")},
            {"StringConcat",   QStringLiteral("node.StringConcat")},
            {"StringLength",   QStringLiteral("node.StringLength")},
            {"StringSubstring",QStringLiteral("node.StringSubstring")},
            {"Print",          QStringLiteral("node.Print")},
            {"BinaryAnd",      QStringLiteral("node.BinaryAnd")},
            {"BinaryOr",       QStringLiteral("node.BinaryOr")},
            {"BinaryXor",      QStringLiteral("node.BinaryXor")},
            {"ShiftLeft",      QStringLiteral("node.ShiftLeft")},
            {"ShiftRight",     QStringLiteral("node.ShiftRight")},
            {"FileReadText",   QStringLiteral("node.FileReadText")},
            {"FileWriteText",  QStringLiteral("node.FileWriteText")},
            {"FileReadBinary", QStringLiteral("node.FileReadBinary")},
            {"RandomInt",      QStringLiteral("node.RandomInt")},
            {"Sleep",          QStringLiteral("node.Sleep")},
        };
    }

    void refreshList() {
        list_->clear();
        for (const auto& entry : nodes_) {
            QString display = QBlock::Translator::tr(entry.trKey);
            if (QBlock::Translator::currentLanguage() == QStringLiteral("zh")) {
                // Show "中文 (English)" format in Chinese mode
                display += QStringLiteral(" (") + entry.typeName + QStringLiteral(")");
            }
            auto* item = new QListWidgetItem(display, list_);
            item->setData(Qt::UserRole, entry.typeName);
        }
    }

    void updateTranslations() {
        setWindowTitle(QBlock::Translator::tr("palette.title"));
        titleLabel_->setText(QStringLiteral("<b>") + QBlock::Translator::tr("palette.nodes") + QStringLiteral("</b>"));
    }

    void onNodeDoubleClicked(QListWidgetItem* item) {
        if (!editor_) return;
        std::string typeName = item->data(Qt::UserRole).toString().toStdString();
        float x = 100.0f + static_cast<float>(std::rand() % 400);
        float y = 100.0f + static_cast<float>(std::rand() % 300);
        editor_->addNode(typeName, x, y);
    }

    QBlock::NodeEditor* editor_;
    QListWidget* list_;
    QLabel* titleLabel_;
    std::vector<NodeEntry> nodes_;
};

#endif // QBLOCK_EXAMPLE_NODE_PALETTE_H