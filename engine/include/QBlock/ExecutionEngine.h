#ifndef QBLOCK_EXECUTION_ENGINE_H
#define QBLOCK_EXECUTION_ENGINE_H

#include "NodeGraph.h"
#include "Variant.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

namespace QBlock {

/// Execution mode for the graph.
enum class ExecutionMode {
    Dataflow,  ///< Data-driven: propagate data changes through connections
    Signal     ///< Signal-driven: follow execution flow pins, pulse-based
};

/// Drives execution of a NodeGraph in either Dataflow or Signal mode.
class ExecutionEngine final {
public:
    ExecutionEngine() = default;

    void setMode(ExecutionMode mode) { mode_ = mode; }
    ExecutionMode mode() const { return mode_; }

    // ---- Dataflow execution ----

    /// Execute the entire graph in dataflow mode.
    /// Starts from nodes with no input connections (source nodes).
    void executeDataflow(NodeGraph& graph) {
        // Topological sort
        auto order = topologicalSort(graph);
        for (auto* node : order) {
            VariantMap inputs = gatherInputs(graph, node);
            VariantMap outputs;
            node->process(inputs, outputs);
            storedOutputs_[node] = std::move(outputs);
        }
    }

    /// Execute a single node and propagate its outputs.
    void executeNodeAndPropagate(NodeGraph& graph, Node* node) {
        VariantMap inputs = gatherInputs(graph, node);
        VariantMap outputs;
        node->process(inputs, outputs);
        storedOutputs_[node] = std::move(outputs);

        // Propagate to downstream nodes
        propagate(graph, node);
    }

    // ---- Signal-driven execution ----

    /// Execute the graph in signal mode, starting from an entry node.
    void executeSignal(NodeGraph& graph, Node* entryNode) {
        std::queue<Node*> nodeQueue;
        nodeQueue.push(entryNode);

        while (!nodeQueue.empty()) {
            auto* node = nodeQueue.front();
            nodeQueue.pop();
            visited_.insert(node->id());

            VariantMap inputs = gatherInputs(graph, node);
            VariantMap outputs;
            node->process(inputs, outputs);
            storedOutputs_[node] = std::move(outputs);

            // Follow exec output connections
            for (const auto& conn : graph.connections()) {
                auto* srcPort = conn->source();
                if (srcPort->parentNode() == node && srcPort->isExecFlow()) {
                    auto* nextNode = conn->target()->parentNode();
                    if (visited_.find(nextNode->id()) == visited_.end()) {
                        nodeQueue.push(nextNode);
                    }
                }
            }
        }
        visited_.clear();
    }

    // ---- Value access ----

    /// Get the last computed output value of a node's port.
    Variant getOutputValue(Node* node, const std::string& portName) const {
        auto it = storedOutputs_.find(node);
        if (it != storedOutputs_.end()) {
            auto vit = it->second.find(portName);
            if (vit != it->second.end())
                return vit->second;
        }
        return Variant();
    }

    /// Reset all stored outputs.
    void reset() {
        storedOutputs_.clear();
        visited_.clear();
    }

private:
    ExecutionMode mode_ = ExecutionMode::Dataflow;
    std::unordered_map<Node*, VariantMap> storedOutputs_;
    std::set<uint64_t> visited_;

    /// Collect input values from upstream connections.
    VariantMap gatherInputs(NodeGraph& graph, Node* node) {
        VariantMap inputs;
        for (const auto& port : node->inputs()) {
            if (port->isExecFlow()) continue;

            // Find connections to this input port
            for (const auto& conn : graph.connections()) {
                if (conn->target() == port.get()) {
                    auto* srcNode = conn->source()->parentNode();
                    auto it = storedOutputs_.find(srcNode);
                    if (it != storedOutputs_.end()) {
                        auto vit = it->second.find(conn->source()->name());
                        if (vit != it->second.end()) {
                            inputs[port->name()] = vit->second;
                        }
                    }
                }
            }
        }
        return inputs;
    }

    /// Propagate outputs from a node to all downstream nodes.
    void propagate(NodeGraph& graph, Node* node) {
        auto& outputs = storedOutputs_[node];
        for (const auto& conn : graph.connections()) {
            if (conn->source()->parentNode() == node && !conn->source()->isExecFlow()) {
                auto* targetNode = conn->target()->parentNode();
                if (targetNode) {
                    executeNodeAndPropagate(graph, targetNode);
                }
            }
        }
    }

    /// Topological sort of nodes based on connections.
    std::vector<Node*> topologicalSort(NodeGraph& graph) {
        std::unordered_map<Node*, int> inDegree;
        std::unordered_map<Node*, std::vector<Node*>> adj;

        for (const auto& node : graph.nodes()) {
            inDegree[node.get()] = 0;
        }

        for (const auto& conn : graph.connections()) {
            if (conn->source()->isExecFlow()) continue;
            auto* src = conn->source()->parentNode();
            auto* tgt = conn->target()->parentNode();
            if (src && tgt) {
                adj[src].push_back(tgt);
                inDegree[tgt]++;
            }
        }

        std::queue<Node*> q;
        for (auto& [node, deg] : inDegree) {
            if (deg == 0) q.push(node);
        }

        std::vector<Node*> result;
        while (!q.empty()) {
            auto* node = q.front(); q.pop();
            result.push_back(node);
            for (auto* next : adj[node]) {
                if (--inDegree[next] == 0)
                    q.push(next);
            }
        }

        return result;
    }
};

} // namespace QBlock

#endif // QBLOCK_EXECUTION_ENGINE_H