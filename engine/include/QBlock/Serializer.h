#ifndef QBLOCK_SERIALIZER_H
#define QBLOCK_SERIALIZER_H

#include "NodeGraph.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QFile>
#include <optional>

namespace QBlock {

struct GraphLoadResult {
    NodeGraph graph;
    bool qtCompatible = false;
};

/// JSON serialization for saving/loading node graphs.
/// Default storage uses qCompress with a 4-byte magic header.
class Serializer final {
public:
    /// Magic header bytes for compressed QBlock files.
    static constexpr quint32 kMagicCompressed  = 0x51424C43; // "QBLC"
    /// Magic for uncompressed JSON (legacy).
    static constexpr quint32 kMagicUncompressed = 0x51424C4A; // "QBLJ"

    /// Serialize a graph to a QJsonDocument.
    static QJsonDocument toJson(const NodeGraph& graph, bool qtCompatible = false) {
        QJsonObject root;
        root[QStringLiteral("version")] = 1;
        root[QStringLiteral("qtCompatible")] = qtCompatible;

        QJsonArray nodesArr;
        for (const auto& node : graph.nodes()) {
            QJsonObject obj;
            obj[QStringLiteral("id")]   = static_cast<qint64>(node->id());
            obj[QStringLiteral("type")] = QString::fromStdString(node->typeName());
            obj[QStringLiteral("label")]= QString::fromStdString(node->label());
            obj[QStringLiteral("posX")] = node->posX();
            obj[QStringLiteral("posY")] = node->posY();

            // Ports
            QJsonArray inPorts, outPorts;
            for (const auto& p : node->inputs()) {
                QJsonObject po;
                po[QStringLiteral("name")] = QString::fromStdString(p->name());
                po[QStringLiteral("type")] = static_cast<int>(static_cast<uint8_t>(p->type()));
                po[QStringLiteral("exec")] = p->isExecFlow();
                inPorts.append(po);
            }
            for (const auto& p : node->outputs()) {
                QJsonObject po;
                po[QStringLiteral("name")] = QString::fromStdString(p->name());
                po[QStringLiteral("type")] = static_cast<int>(static_cast<uint8_t>(p->type()));
                po[QStringLiteral("exec")] = p->isExecFlow();
                outPorts.append(po);
            }
            obj[QStringLiteral("inputs")]  = inPorts;
            obj[QStringLiteral("outputs")] = outPorts;
            nodesArr.append(obj);
        }
        root[QStringLiteral("nodes")] = nodesArr;

        QJsonArray connsArr;
        for (const auto& conn : graph.connections()) {
            QJsonObject obj;
            obj[QStringLiteral("id")]     = static_cast<qint64>(conn->id());
            obj[QStringLiteral("sourceNode")] = static_cast<qint64>(conn->source()->parentNode()->id());
            obj[QStringLiteral("sourcePort")] = QString::fromStdString(conn->source()->name());
            obj[QStringLiteral("targetNode")] = static_cast<qint64>(conn->target()->parentNode()->id());
            obj[QStringLiteral("targetPort")] = QString::fromStdString(conn->target()->name());
            connsArr.append(obj);
        }
        root[QStringLiteral("connections")] = connsArr;

        return QJsonDocument(root);
    }

    /// Deserialize a graph from a QJsonDocument.
    template <typename Factory>
    static std::optional<GraphLoadResult> fromJson(const QJsonDocument& doc, Factory nodeFactory) {
        if (!doc.isObject()) return std::nullopt;

        QJsonObject root = doc.object();
        GraphLoadResult result;

        if (root.contains(QStringLiteral("qtCompatible"))) {
            result.qtCompatible = root[QStringLiteral("qtCompatible")].toBool();
        }

        QJsonArray nodesArr = root[QStringLiteral("nodes")].toArray();
        std::unordered_map<uint64_t, Node*> nodeMap;

        for (const auto& val : nodesArr) {
            QJsonObject obj = val.toObject();
            uint64_t id = static_cast<uint64_t>(obj[QStringLiteral("id")].toInteger());
            std::string type = obj[QStringLiteral("type")].toString().toStdString();

            auto node = nodeFactory(type);
            if (!node) continue;

            node->setId(id);
            node->setPosition(
                static_cast<float>(obj[QStringLiteral("posX")].toDouble()),
                static_cast<float>(obj[QStringLiteral("posY")].toDouble())
            );
            if (obj.contains(QStringLiteral("label")))
                node->setLabel(obj[QStringLiteral("label")].toString().toStdString());

            auto* ptr = result.graph.addNode(std::move(node));
            ptr->setId(id);
            nodeMap[id] = ptr;
        }

        QJsonArray connsArr = root[QStringLiteral("connections")].toArray();
        for (const auto& val : connsArr) {
            QJsonObject obj = val.toObject();
            uint64_t srcNodeId = static_cast<uint64_t>(obj[QStringLiteral("sourceNode")].toInteger());
            uint64_t tgtNodeId = static_cast<uint64_t>(obj[QStringLiteral("targetNode")].toInteger());
            std::string srcPort = obj[QStringLiteral("sourcePort")].toString().toStdString();
            std::string tgtPort = obj[QStringLiteral("targetPort")].toString().toStdString();

            auto* srcNode = nodeMap[srcNodeId];
            auto* tgtNode = nodeMap[tgtNodeId];
            if (!srcNode || !tgtNode) continue;

            Port* srcPortPtr = nullptr;
            for (const auto& p : srcNode->outputs()) {
                if (p->name() == srcPort) { srcPortPtr = p.get(); break; }
            }
            Port* tgtPortPtr = nullptr;
            for (const auto& p : tgtNode->inputs()) {
                if (p->name() == tgtPort) { tgtPortPtr = p.get(); break; }
            }

            if (srcPortPtr && tgtPortPtr)
                result.graph.connect(srcPortPtr, tgtPortPtr);
        }

        return result;
    }

    /// Serialize to compressed bytes (default).
    /// Format: [4-byte magic] [4-byte original size] [qCompressed JSON]
    static QByteArray toBytes(const NodeGraph& graph, bool qtCompatible = false) {
        QJsonDocument doc = toJson(graph, qtCompatible);
        QByteArray json = doc.toJson(QJsonDocument::Compact);
        QByteArray compressed = qCompress(json);

        QByteArray result;
        QDataStream stream(&result, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << kMagicCompressed;
        stream << static_cast<quint32>(json.size());
        result.append(compressed);
        return result;
    }

    /// Serialize to uncompressed JSON bytes (legacy).
    static QByteArray toBytesUncompressed(const NodeGraph& graph) {
        QJsonDocument doc = toJson(graph);
        return doc.toJson(QJsonDocument::Indented);
    }

    /// Deserialize from bytes (auto-detects compressed vs uncompressed).
    template <typename Factory>
    static std::optional<GraphLoadResult> fromBytes(const QByteArray& data, Factory nodeFactory) {
        if (data.isEmpty()) return std::nullopt;

        // Check for compressed magic header
        if (data.size() >= 8) {
            QDataStream stream(data);
            stream.setByteOrder(QDataStream::LittleEndian);
            quint32 magic;
            stream >> magic;

            if (magic == kMagicCompressed) {
                quint32 originalSize;
                stream >> originalSize;
                // Remaining bytes are the compressed data (skip 8 header bytes)
                QByteArray compressed = data.mid(8);
                QByteArray json = qUncompress(compressed);
                if (json.isEmpty()) return std::nullopt;
                QJsonDocument doc = QJsonDocument::fromJson(json);
                return fromJson(doc, nodeFactory);
            }
        }

        // Fallback: try as plain JSON (legacy format or .qblock.json)
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) return std::nullopt;
        return fromJson(doc, nodeFactory);
    }
};

} // namespace QBlock

#endif // QBLOCK_SERIALIZER_H