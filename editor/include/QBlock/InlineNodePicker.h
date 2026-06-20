#ifndef QBLOCK_INLINE_NODE_PICKER_H
#define QBLOCK_INLINE_NODE_PICKER_H

#include <QBlock/DataType.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include <vector>

namespace QBlock {

class InlineNodePicker : public QGraphicsProxyWidget {
    Q_OBJECT
public:
    using TypeLister = std::function<std::vector<std::string>()>;

    InlineNodePicker(QGraphicsScene* scene, const QPointF& pos,
                     DataType filterType, TypeLister typeLister)
        : QGraphicsProxyWidget(nullptr)
        , filterType_(filterType)
        , typeLister_(std::move(typeLister))
    {
        setPos(pos);
        // Ensure picker is always on top
        setZValue(1000);

        auto* widget = new QWidget();
        widget->setFixedSize(280, 360);

        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(6, 6, 6, 6);
        layout->setSpacing(4);

        searchEdit_ = new QLineEdit(widget);
        searchEdit_->setPlaceholderText(Translator::tr("picker.search"));
        layout->addWidget(searchEdit_);

        list_ = new QListWidget(widget);
        list_->setIconSize(QSize(32, 32));
        list_->setSpacing(4);
        list_->setViewMode(QListView::IconMode);
        list_->setResizeMode(QListView::Adjust);
        list_->setMovement(QListView::Static);
        list_->setGridSize(QSize(60, 60));
        layout->addWidget(list_, 1);

        setWidget(widget);
        scene->addItem(this);

        applyThemeStyle();

        connect(searchEdit_, &QLineEdit::textChanged, this, [this]() {
            populateList();
        });

        connect(list_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            if (item) {
                selectedType_ = item->data(Qt::UserRole).toString().toStdString();
                emit nodeSelected(selectedType_);
            }
        });

        connect(list_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
            if (item) {
                selectedType_ = item->data(Qt::UserRole).toString().toStdString();
                emit nodeSelected(selectedType_);
            }
        });

        ThemeManager::onThemeChanged([this]() { applyThemeStyle(); });

        populateList();
        searchEdit_->setFocus();
    }

    ~InlineNodePicker() override {
        if (scene()) {
            scene()->removeItem(this);
        }
    }

    std::string selectedType() const { return selectedType_; }

signals:
    void nodeSelected(const std::string& type);

private:
    void applyThemeStyle() {
        bool isDark = (ThemeManager::instance().currentTheme() == ThemeMode::Dark);
        QColor bg = ThemeManager::dialogBg();
        QColor border = ThemeManager::dialogBorder();
        QColor listBg = ThemeManager::listBg();
        QColor itemHover = ThemeManager::listItemHover();
        QColor itemSel = ThemeManager::listItemSelected();
        QColor text = ThemeManager::textPrimary();

        QString style = QString(
            "QWidget { background: %1; border: 1px solid %2; border-radius: 8px; }"
            "QLineEdit { background: %3; color: %4; border: 1px solid %2; "
            "  padding: 4px 8px; border-radius: 4px; }"
            "QListWidget { background: %3; color: %4; border: 1px solid %2; "
            "  border-radius: 4px; padding: 4px; }"
            "QListWidget::item { border-radius: 4px; padding: 4px; }"
            "QListWidget::item:selected { background: %5; color: #fff; }"
            "QListWidget::item:hover { background: %6; }"
        ).arg(bg.name(), border.name(), listBg.name(), text.name(),
              itemSel.name(), itemHover.name());

        widget()->setStyleSheet(style);
    }

    void populateList() {
        list_->clear();
        if (!typeLister_) return;

        std::vector<std::string> types = typeLister_();
        QString searchText = searchEdit_->text().toLower();

        for (const auto& typeName : types) {
            auto probe = Builtin::createBuiltinNode(typeName);
            if (!probe) continue;

            bool matchFilter = (filterType_ == DataType::Generic);
            if (!matchFilter) {
                for (const auto& input : probe->inputs()) {
                    if (typesCompatible(filterType_, input->type())) {
                        matchFilter = true;
                        break;
                    }
                }
            }
            if (!matchFilter) continue;

            QString trKey = QStringLiteral("node.") + QString::fromStdString(typeName);
            QString display = Translator::tr(trKey);
            if (display == trKey) display = QString::fromStdString(typeName);

            bool matchSearch = searchText.isEmpty();
            if (!matchSearch) {
                matchSearch = display.toLower().contains(searchText) ||
                              QString::fromStdString(typeName).toLower().contains(searchText);
            }
            if (!matchSearch) continue;

            auto* item = new QListWidgetItem(list_);
            item->setIcon(QIcon(QBlock::createNodeIcon(typeName)));
            item->setToolTip(display);
            item->setData(Qt::UserRole, QString::fromStdString(typeName));
        }
    }

    QListWidget* list_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    DataType filterType_ = DataType::Generic;
    TypeLister typeLister_;
    std::string selectedType_;
};

} // namespace QBlock

#endif // QBLOCK_INLINE_NODE_PICKER_H