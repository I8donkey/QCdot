#ifndef QBLOCK_NODEGRAPH_H
#define QBLOCK_NODEGRAPH_H

#include "Node.h"
#include "Connection.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace QBlock {

/// Container for a graph of nodes and their connections.
/// Owns all nodes and connections within a single graph.
class NodeGraph final {
public:
    NodeGraph() = default;
    ~NodeGraph() = default;

    // Move operations (required because user-declared destructor suppresses implicit move)
    NodeGraph(NodeGraph&&) = default;
    NodeGraph& operator=(NodeGraph&&) = default;

    // Copy is not allowed (unique_ptr members)
    NodeGraph(const NodeGraph&) = delete;
    NodeGraph& operator=(const NodeGraph&) = delete;

    // ---- Node management ----

    /// Add a node to the graph. Returns raw pointer (graph owns it).
    Node* addNode(std::unique_ptr<Node> node) {
        node->setId(nextNodeId_++);
        node->setGraph(this);
        auto* ptr = node.get();
        nodes_.push_back(std::move(node));
        return ptr;
    }

    /// Remove a node and all its connections.
    void removeNode(Node* node) {
        // Remove all connections involving this node
        auto it = connections_.begin();
        while (it != connections_.end()) {
            auto* conn = it->get();
            if (conn->source()->parentNode() == node ||
                conn->target()->parentNode() == node) {
                it = connections_.erase(it);
            } else {
                ++it;
            }
        }
        // Remove the node
        for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
            if (it->get() == node) {
                nodes_.erase(it);
                return;
            }
        }
    }

    /// Find a node by ID.
    Node* findNode(uint64_t id) const {
        for (const auto& n : nodes_) {
            if (n->id() == id) return n.get();
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Node>>& nodes() const { return nodes_; }

    // ---- Connection management ----

    /// Connect an output port to an input port. Returns nullptr on failure.
    Connection* connect(Port* source, Port* target) {
        if (!source || !target) return nullptr;
        if (source->direction() != PortDirection::Output) return nullptr;
        if (target->direction() != PortDirection::Input) return nullptr;
        if (!typesCompatible(source->type(), target->type())) return nullptr;

        auto conn = std::make_unique<Connection>(source, target, nextConnectionId_++);
        auto* ptr = conn.get();
        connections_.push_back(std::move(conn));
        return ptr;
    }

    /// Remove a connection.
    void disconnect(Connection* conn) {
        for (auto it = connections_.begin(); it != connections_.end(); ++it) {
            if (it->get() == conn) {
                connections_.erase(it);
                return;
            }
        }
    }

    void disconnect(Port* source, Port* target) {
        for (auto it = connections_.begin(); it != connections_.end(); ++it) {
            if (it->get()->source() == source && it->get()->target() == target) {
                connections_.erase(it);
                return;
            }
        }
    }

    const std::vector<std::unique_ptr<Connection>>& connections() const { return connections_; }

    /// Find connections attached to a port.
    std::vector<Connection*> connectionsForPort(Port* port) const {
        std::vector<Connection*> result;
        for (const auto& c : connections_) {
            if (c->source() == port || c->target() == port)
                result.push_back(c.get());
        }
        return result;
    }

    /// Clear all nodes and connections.
    void clear() {
        nodes_.clear();
        connections_.clear();
        nextNodeId_ = 1;
        nextConnectionId_ = 1;
    }

    bool empty() const { return nodes_.empty(); }
    size_t nodeCount() const { return nodes_.size(); }
    size_t connectionCount() const { return connections_.size(); }

private:
    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<std::unique_ptr<Connection>> connections_;
    uint64_t nextNodeId_ = 1;
    uint64_t nextConnectionId_ = 1;
};

} // namespace QBlock

#endif // QBLOCK_NODEGRAPH_H