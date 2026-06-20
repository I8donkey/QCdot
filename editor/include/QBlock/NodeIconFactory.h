#ifndef QBLOCK_NODE_ICON_FACTORY_H
#define QBLOCK_NODE_ICON_FACTORY_H

#include <QBlock/BuiltinNodes.h>
#include <QBlock/DataType.h>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <string>

namespace QBlock {

/// Creates a 32x32 icon representing a node type by its port configuration.
inline QPixmap createNodeIcon(const std::string& typeName) {
    using namespace Builtin;

    QPixmap icon(32, 32);
    icon.fill(Qt::transparent);
    QPainter p(&icon);
    p.setRenderHint(QPainter::Antialiasing);

    // Background rounded-rect (dark panel style)
    QColor bgColor(38, 40, 52);
    p.setBrush(bgColor);
    p.setPen(QPen(QColor(100, 100, 115), 1.5));
    p.drawRoundedRect(2, 2, 28, 28, 4, 4);

    auto probe = createBuiltinNode(typeName);

    if (probe) {
        const auto& inputs = probe->inputs();
        int count = static_cast<int>(inputs.size());
        if (count > 0) {
            float spacing = 24.0f / std::max(1, count);
            float baseY = (32.0f - (count - 1) * spacing) / 2.0f - 1.5f;
            for (int i = 0; i < count; ++i) {
                QColor portColor = inputs[static_cast<size_t>(i)]->color();
                p.setBrush(portColor);
                p.setPen(QPen(portColor.darker(130), 1));
                p.drawEllipse(QPointF(5, baseY + i * spacing), 3.5f, 3.5f);
            }
        } else {
            p.setBrush(QColor(80, 80, 90));
            p.setPen(Qt::NoPen);
            p.drawRect(3, 13, 3, 6);
        }

        const auto& outputs = probe->outputs();
        int ocount = static_cast<int>(outputs.size());
        if (ocount > 0) {
            float spacing = 24.0f / std::max(1, ocount);
            float baseY = (32.0f - (ocount - 1) * spacing) / 2.0f - 1.5f;
            for (int i = 0; i < ocount; ++i) {
                QColor portColor = outputs[static_cast<size_t>(i)]->color();
                p.setBrush(portColor);
                p.setPen(QPen(portColor.darker(130), 1));
                p.drawEllipse(QPointF(27, baseY + i * spacing), 3.5f, 3.5f);
            }
        }
    }

    QColor textColor(200, 200, 220);
    p.setPen(QPen(textColor, 1));
    p.setFont(QFont("Consolas", 10, QFont::Bold));

    QString letter;
    if (!typeName.empty()) {
        char c = typeName[0];
        if (c >= 'a' && c <= 'z') c -= 32;
        letter = QChar(c);
    } else {
        letter = QStringLiteral("?");
    }
    p.drawText(QRectF(6, 6, 20, 20), Qt::AlignCenter, letter);

    return icon;
}

} // namespace QBlock

#endif // QBLOCK_NODE_ICON_FACTORY_H
