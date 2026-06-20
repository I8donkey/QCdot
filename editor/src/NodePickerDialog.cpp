#include <QBlock/NodePickerDialog.h>
#include <QBlock/BuiltinNodes.h>
#include <QBlock/DataType.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>

namespace QBlock {

NodePickerDialog::NodePickerDialog(DataType sourceType, bool isInput,
                                   std::function<std::vector<std::string>()> typeLister,
                                   QWidget* parent)
    : QDialog(parent)
    , selectedType_()
{
    setWindowTitle(Translator::tr("picker.title"));
    setMinimumSize(520, 400);

    auto* layout = new QVBoxLayout(this);

    promptLabel_ = new QLabel(Translator::tr("picker.prompt"), this);
    layout->addWidget(promptLabel_);

    list_ = new QListWidget(this);
    list_->setIconSize(QSize(32, 32));
    list_->setSpacing(4);

    // In Chinese mode: icon-only display in grid view
    if (Translator::currentLanguage() == QStringLiteral("zh")) {
        list_->setViewMode(QListView::IconMode);
        list_->setResizeMode(QListView::Adjust);
        list_->setMovement(QListView::Static);
        list_->setSpacing(12);
        list_->setGridSize(QSize(76, 76));
    } else {
        list_->setViewMode(QListView::ListMode);
    }
    layout->addWidget(list_, 1);

    // Button row with Cancel
    auto* buttonRow = new QHBoxLayout();
    cancelBtn_ = new QPushButton(Translator::tr("picker.cancel"), this);
    buttonRow->addStretch(1);
    buttonRow->addWidget(cancelBtn_);
    layout->addLayout(buttonRow);

    connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);
    connect(list_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            selectedType_ = item->data(Qt::UserRole).toString().toStdString();
            accept();
        }
    });

    applyThemeStyle();
    ThemeManager::onThemeChanged([this]() { applyThemeStyle(); });

    populateList(sourceType, isInput, std::move(typeLister));
}

void NodePickerDialog::applyThemeStyle() {
    bool isDark = (ThemeManager::instance().currentTheme() == ThemeMode::Dark);
    QColor bg = ThemeManager::dialogBg();
    QColor border = ThemeManager::dialogBorder();
    QColor listBg = ThemeManager::listBg();
    QColor itemHover = ThemeManager::listItemHover();
    QColor itemSel = ThemeManager::listItemSelected();
    QColor text = ThemeManager::textPrimary();

    QString style = QString(
        "QDialog { background: %1; }"
        "QLabel { color: %2; font-size: 12pt; padding: 6px; }"
        "QListWidget { background: %3; color: %4; border: 1px solid %5; border-radius: 4px; padding: 6px; }"
        "QListWidget::item:selected { background: %6; color: #fff; border-radius: 4px; }"
        "QListWidget::item:hover { background: %7; color: #fff; border-radius: 4px; }"
        "QPushButton { background: %8; color: %9; border: 1px solid %10; padding: 6px 16px; border-radius: 4px; }"
        "QPushButton:hover { background: %11; color: #fff; }"
    ).arg(bg.name(), text.name(), listBg.name(), text.name(), border.name(),
          itemSel.name(), itemHover.name(), border.name(), text.name(), border.name(),
          itemHover.name());

    setStyleSheet(style);
}

void NodePickerDialog::populateList(DataType sourceType, bool /*isInput*/,
                                    std::function<std::vector<std::string>()> typeLister) {
    if (!typeLister) return;
    list_->clear();

    std::vector<std::string> types = typeLister();

    // Filter: only include types that can accept sourceType as input
    // OR (for leaf nodes / display nodes) have at least one input
    std::vector<std::string> compatibleTypes;
    for (const auto& typeName : types) {
        auto probe = Builtin::createBuiltinNode(typeName);
        if (!probe) continue;

        // A node is compatible if it has at least one input port whose type
        // is compatible with sourceType, OR if sourceType is Generic (show all)
        bool compatible = (sourceType == DataType::Generic);
        if (!compatible) {
            for (const auto& input : probe->inputs()) {
                if (typesCompatible(sourceType, input->type())) {
                    compatible = true;
                    break;
                }
            }
        }
        if (compatible) {
            compatibleTypes.push_back(typeName);
        }
    }

    std::sort(compatibleTypes.begin(), compatibleTypes.end());

    for (const auto& typeName : compatibleTypes) {
        auto* item = new QListWidgetItem(list_);
        item->setIcon(QIcon(QBlock::createNodeIcon(typeName)));

        // In Chinese-only mode: icon only, no text, tooltip is translated name
        if (Translator::currentLanguage() != QStringLiteral("zh")) {
            item->setText(QString::fromStdString(typeName));
            item->setSizeHint(QSize(180, 32));
        } else {
            QString trKey = QStringLiteral("node.") + QString::fromStdString(typeName);
            QString trName = Translator::tr(trKey);
            if (trName == trKey) trName = QString::fromStdString(typeName);
            item->setToolTip(trName);
            item->setSizeHint(QSize(76, 76));
        }

        item->setData(Qt::UserRole, QString::fromStdString(typeName));
    }
}

} // namespace QBlock
