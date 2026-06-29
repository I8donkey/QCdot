#ifndef QBLOCK_INLINE_NODE_PICKER_H
#define QBLOCK_INLINE_NODE_PICKER_H

#include <QBlock/DataType.h>
#include <QBlock/Translator.h>
#include <QBlock/NodeIconFactory.h>
#include <QBlock/ThemeManager.h>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsPathItem>
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
                     DataType filterType, TypeLister typeLister,
                     QPointF sourcePortPos = QPointF())
        : QGraphicsProxyWidget(nullptr)
        , filterType_(filterType)
        , typeLister_(std::move(typeLister))
        , sourcePortPos_(sourcePortPos)
        , hasSourcePort_(sourcePortPos != QPointF())
    {
        setPos(pos);
        setZValue(1000);
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

        auto* widget = new QWidget();
        widget->setFixedSize(280, 360);
        widget->setAttribute(Qt::WA_TranslucentBackground, true);

        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(6, 6, 6, 6);
        layout->setSpacing(4);

        searchEdit_ = new QLineEdit(widget);
        searchEdit_->setPlaceholderText(Translator::tr("picker.search"));
        layout->addWidget(searchEdit_);

        list_ = new QListWidget(widget);
        list_->setIconSize(QSize(32, 32));
        list_->setSpacing(4);
        list_->setViewMode(QListView::ListMode);
        list_->setResizeMode(QListView::Adjust);
        list_->setMovement(QListView::Static);
        layout->addWidget(list_, 1);

        setWidget(widget);
        scene->addItem(this);

        if (hasSourcePort_) {
            dashedLine_ = new QGraphicsPathItem(this);
            dashedLine_->setZValue(-1);
            updateDashedLine();
        }

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

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) {
        if (change == QGraphicsItem::ItemPositionChange) {
            updateDashedLine();
        }
        return QGraphicsProxyWidget::itemChange(change, value);
    }

signals:
    void nodeSelected(const std::string& type);

private:
    void applyThemeStyle() {
        bool isDark = (ThemeManager::instance().currentTheme() == ThemeMode::Dark);
        QColor bg(38, 40, 52, 200);
        QColor border(100, 100, 115, 200);
        QColor listBg(45, 48, 62, 180);
        QColor itemHover(60, 64, 80, 200);
        QColor itemSel(80, 120, 200, 200);
        QColor text(220, 220, 240);

        QString style = QString(
            "QWidget { background: rgba(%1, %2, %3, %4); border: 1px solid rgba(%5, %6, %7, %8); border-radius: 8px; }"
            "QLineEdit { background: rgba(%9, %10, %11, %12); color: %13; border: 1px solid rgba(%14, %15, %16, %17); "
            "  padding: 4px 8px; border-radius: 4px; }"
            "QListWidget { background: rgba(%9, %10, %11, %12); color: %13; border: 1px solid rgba(%14, %15, %16, %17); "
            "  border-radius: 4px; padding: 4px; }"
            "QListWidget::item { border-radius: 4px; padding: 4px; }"
            "QListWidget::item:selected { background: rgba(%18, %19, %20, %21); color: #fff; }"
            "QListWidget::item:hover { background: rgba(%22, %23, %24, %25); }"
        ).arg(bg.red()).arg(bg.green()).arg(bg.blue()).arg(bg.alpha())
         .arg(border.red()).arg(border.green()).arg(border.blue()).arg(border.alpha())
         .arg(listBg.red()).arg(listBg.green()).arg(listBg.blue()).arg(listBg.alpha())
         .arg(text.name())
         .arg(border.red()).arg(border.green()).arg(border.blue()).arg(border.alpha())
         .arg(itemSel.red()).arg(itemSel.green()).arg(itemSel.blue()).arg(itemSel.alpha())
         .arg(itemHover.red()).arg(itemHover.green()).arg(itemHover.blue()).arg(itemHover.alpha());

        widget()->setStyleSheet(style);

        if (dashedLine_) {
            QColor lineColor = colorForType(filterType_);
            QPen pen(lineColor);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(2);
            dashedLine_->setPen(pen);
        }
    }

    void updateDashedLine() {
        if (!dashedLine_ || !hasSourcePort_) return;

        QPointF widgetPos = pos();
        QRectF widgetRect = boundingRect();

        QPointF pickerEdge = widgetPos + QPointF(widgetRect.width() / 2, widgetRect.top());

        QPainterPath path;
        path.moveTo(sourcePortPos_);
        path.lineTo(pickerEdge);

        dashedLine_->setPath(path);
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
            item->setText(display);
            item->setToolTip(display);
            item->setData(Qt::UserRole, QString::fromStdString(typeName));
        }
    }

    QListWidget* list_ = nullptr;
    QLineEdit* searchEdit_ = nullptr;
    DataType filterType_ = DataType::Generic;
    TypeLister typeLister_;
    std::string selectedType_;
    QPointF sourcePortPos_;
    bool hasSourcePort_ = false;
    QGraphicsPathItem* dashedLine_ = nullptr;
};

} // namespace QBlock

#endif // QBLOCK_INLINE_NODE_PICKER_H
