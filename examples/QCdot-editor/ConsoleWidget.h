#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QString>
#include <QBlock/Translator.h>

class ConsoleWidget : public QWidget {
    Q_OBJECT
public:
    ConsoleWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setObjectName(QStringLiteral("ConsoleWidget"));
        
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        
        textEdit_ = new QPlainTextEdit(this);
        textEdit_->setReadOnly(true);
        textEdit_->setFont(QFont("Consolas", 9));
        textEdit_->setLineWrapMode(QPlainTextEdit::NoWrap);
        layout->addWidget(textEdit_, 1);
        
        auto* btnLayout = new QHBoxLayout();
        btnLayout->setContentsMargins(4, 2, 4, 4);
        btnLayout->setSpacing(4);
        
        clearBtn_ = new QPushButton(QBlock::Translator::tr("console.clear"), this);
        clearBtn_->setFixedHeight(24);
        btnLayout->addWidget(clearBtn_);
        
        btnLayout->addStretch();
        
        layout->addLayout(btnLayout);
        
        connect(clearBtn_, &QPushButton::clicked, this, [this]() {
            textEdit_->clear();
        });
    }
    
    void appendText(const QString& text) {
        textEdit_->appendPlainText(text);
        textEdit_->verticalScrollBar()->setValue(textEdit_->verticalScrollBar()->maximum());
    }
    
    void setDarkMode(bool dark) {
        if (dark) {
            textEdit_->setStyleSheet(
                "QPlainTextEdit { background: #1e1e2e; color: #d4d4d4; border: none; }"
            );
            clearBtn_->setStyleSheet(
                "QPushButton { background: #333; color: #ccc; border: 1px solid #444; "
                "  border-radius: 3px; padding: 2px 8px; }"
                "QPushButton:hover { background: #444; }"
            );
        } else {
            textEdit_->setStyleSheet(
                "QPlainTextEdit { background: #fff; color: #333; border: none; }"
            );
            clearBtn_->setStyleSheet(
                "QPushButton { background: #e0e0e5; color: #333; border: 1px solid #ccc; "
                "  border-radius: 3px; padding: 2px 8px; }"
                "QPushButton:hover { background: #d0d0d5; }"
            );
        }
    }
    
private:
    QPlainTextEdit* textEdit_ = nullptr;
    QPushButton* clearBtn_ = nullptr;
};

#endif // CONSOLE_WIDGET_H
