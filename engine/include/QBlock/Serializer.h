#ifndef QBLOCK_SERIALIZER_H
#define QBLOCK_SERIALIZER_H

#include "NodeGraph.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <optional>

namespace QBlock {

/// JSON serialization for saving/loading node graphs.
class Serializer final {
public:
    /// Serialize a graph to a JSON string.
    static QJsonDocument toJson(const NodeGraph& graph) {
        QJsonObject root;
        root[QStringLiteral("version")] = 1;

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

    /// Deserialize a graph from a JSON document.
    /// Nodes are created via a factory function: given a type name, return a new node.
    template <typename Factory>
    static std::optional<NodeGraph> fromJson(const QJsonDocument& doc, Factory nodeFactory) {
        if (!doc.isObject()) return std::nullopt;

        QJsonObject root = doc.object();
        NodeGraph graph;

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

            auto* ptr = graph.addNode(std::move(node));
            ptr->setId(id); // override the auto-assigned ID
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
                graph.connect(srcPortPtr, tgtPortPtr);
        }

        return graph;
    }

    /// Convenience: serialize to string.
    static QByteArray toBytes(const NodeGraph& graph) {
        return toJson(graph).toJson(QJsonDocument::Indented);
    }
};

} // namespace QBlock

#endif // QBLOCK_SERIALIZER_H