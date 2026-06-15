#ifndef QBLOCK_EXAMPLE_NODE_PALETTE_H
#define QBLOCK_EXAMPLE_NODE_PALETTE_H

#include <QBlock/NodeEditor.h>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>

/// Side panel with a list of available nodes for drag-and-drop.
class NodePalette : public QWidget {
    Q_OBJECT
public:
    explicit NodePalette(QBlock::NodeEditor* editor, QWidget* parent = nullptr)
        : QWidget(parent), editor_(editor)
    {
        setWindowTitle("Node Palette");
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);

        auto* title = new QLabel("<b>Nodes</b>");
        title->setStyleSheet("color: #ccc; padding: 4px;");
        layout->addWidget(title);

        list_ = new QListWidget(this);
        list_->setStyleSheet(
            "QListWidget { background: #2a2a32; border: 1px solid #444; color: #ccc; }"
            "QListWidget::item { padding: 4px 8px; }"
            "QListWidget::item:hover { background: #3a3a48; }"
            "QListWidget::item:selected { background: #4a4a5a; }"
        );
        layout->addWidget(list_);

        populateNodes();

        connect(list_, &QListWidget::itemDoubleClicked, this, &NodePalette::onNodeDoubleClicked);
    }

private:
    void populateNodes() {
        list_->addItem("Add"); list_->addItem("Subtract");
        list_->addItem("Multiply"); list_->addItem("Divide");
        list_->addItem("Modulo"); list_->addItem("Power"); list_->addItem("Sqrt");
        list_->addItem("Equal"); list_->addItem("GreaterThan"); list_->addItem("LessThan");
        list_->addItem("And"); list_->addItem("Or"); list_->addItem("Not");
        list_->addItem("IfElse"); list_->addItem("IfElseBranch");
        list_->addItem("ConstantInt"); list_->addItem("ConstantFloat");
        list_->addItem("ConstantBool"); list_->addItem("ConstantString");
        list_->addItem("IntToFloat"); list_->addItem("FloatToInt"); list_->addItem("ToString");
        list_->addItem("StringConcat"); list_->addItem("StringLength"); list_->addItem("StringSubstring");
        list_->addItem("Print");
        list_->addItem("BinaryAnd"); list_->addItem("BinaryOr");
        list_->addItem("BinaryXor"); list_->addItem("ShiftLeft"); list_->addItem("ShiftRight");
        list_->addItem("FileReadText"); list_->addItem("FileWriteText"); list_->addItem("FileReadBinary");
        list_->addItem("RandomInt"); list_->addItem("Sleep");
    }

    void onNodeDoubleClicked(QListWidgetItem* item) {
        if (!editor_) return;
        std::string typeName = item->text().toStdString();
        float x = 100.0f + static_cast<float>(std::rand() % 400);
        float y = 100.0f + static_cast<float>(std::rand() % 300);
        editor_->addNode(typeName, x, y);
    }

    QBlock::NodeEditor* editor_;
    QListWidget* list_;
};

#endif // QBLOCK_EXAMPLE_NODE_PALETTE_H