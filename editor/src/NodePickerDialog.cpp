#include <QBlock/NodePickerDialog.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeGraph.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>

namespace QBlock {

NodePickerDialog::NodePickerDialog(DataType sourceType, bool isInput,
                                   std::function<std::vector<std::string>()> typeLister,
                                   QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(Translator::tr("picker.title"));
    setFixedSize(320, 420);
    setModal(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    auto* titleLabel = new QLabel(Translator::tr("picker.prompt"));
    titleLabel->setStyleSheet("color: #ccc; font-size: 13px; padding: 4px;");
    layout->addWidget(titleLabel);

    list_ = new QListWidget(this);
    list_->setStyleSheet(
        "QListWidget { background: #2a2a32; border: 1px solid #444; color: #ccc; }"
        "QListWidget::item { padding: 6px 8px; border-bottom: 1px solid #333; }"
        "QListWidget::item:hover { background: #3a3a48; }"
        "QListWidget::item:selected { background: #4a4a5a; }"
    );
    list_->setIconSize(QSize(24, 24));
    list_->setWordWrap(false);
    layout->addWidget(list_);

    auto* cancelBtn = new QPushButton(Translator::tr("picker.cancel"));
    cancelBtn->setStyleSheet(
        "QPushButton { background: #444; color: #ccc; border: 1px solid #555; "
        "border-radius: 3px; padding: 6px 16px; }"
        "QPushButton:hover { background: #555; }"
    );
    layout->addWidget(cancelBtn);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    populateList(sourceType, isInput, std::move(typeLister));

    connect(list_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            selectedType_ = item->data(Qt::UserRole).toString().toStdString();
            accept();
        }
    });
}

void NodePickerDialog::populateList(DataType sourceType, bool isInput,
                                    std::function<std::vector<std::string>()> typeLister) {
    if (!typeLister) return;

    auto allTypes = typeLister();
    QColor typeColor = colorForType(sourceType);

    for (const auto& typeName : allTypes) {
        // Generate a simple colored icon for each node type
        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter p(&icon);
        p.setRenderHint(QPainter::Antialiasing);

        // Draw a rounded rect icon with the data type color
        QColor bg = typeColor.lighter(180);
        bg.setAlpha(180);
        p.setBrush(bg);
        p.setPen(QPen(typeColor, 1.5));
        p.drawRoundedRect(2, 2, 20, 20, 4, 4);

        // Draw a small symbol based on first letter
        p.setFont(QFont("Consolas", 10, QFont::Bold));
        p.setPen(typeColor);
        p.drawText(QRect(2, 2, 20, 20), Qt::AlignCenter, QString::fromStdString(typeName.substr(0, 1)));
        p.end();

        QString trKey = QStringLiteral("node.") + QString::fromStdString(typeName);
        QString displayName = Translator::tr(trKey);
        if (displayName == trKey) {
            displayName = QString::fromStdString(typeName);
        }

        auto* item = new QListWidgetItem(QIcon(icon), displayName, list_);
        item->setData(Qt::UserRole, QString::fromStdString(typeName));
        item->setToolTip(QStringLiteral("Type: ") + QString::fromStdString(typeName));
    }
}

} // namespace QBlock