#ifndef QBLOCK_NODE_H
#define QBLOCK_NODE_H

#include "Port.h"
#include "Variant.h"
#include <QString>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace QBlock {

/// Map of port names to values, used for node I/O.
using VariantMap = std::map<std::string, Variant>;

class NodeGraph;

/// Abstract base class for all nodes.
/// Subclass this and override process() to create custom nodes.
class Node {
public:
    Node() = default;
    virtual ~Node() = default;

    // ---- Subclass API (override these) ----

    /// Unique type name for this node (e.g. "Add", "IfElse").
    virtual std::string typeName() const = 0;

    /// Human-readable title shown on the node widget.
    virtual std::string nodeTitle() const { return typeName(); }

    /// Execute the node's logic. Receives input values, must populate output values.
    virtual void process(const VariantMap& inputs, VariantMap& outputs) = 0;

    // ---- Port management ----

    Port* addInput(const std::string& name, DataType type) {
        auto port = std::make_unique<Port>(name, type, PortDirection::Input, this);
        auto* ptr = port.get();
        inputs_.push_back(std::move(port));
        return ptr;
    }

    Port* addOutput(const std::string& name, DataType type) {
        auto port = std::make_unique<Port>(name, type, PortDirection::Output, this);
        auto* ptr = port.get();
        outputs_.push_back(std::move(port));
        return ptr;
    }

    Port* addExecInput(const std::string& name = "exec") {
        auto port = std::make_unique<Port>(name, PortDirection::Input, this);
        auto* ptr = port.get();
        inputs_.push_back(std::move(port));
        return ptr;
    }

    Port* addExecOutput(const std::string& name = "exec") {
        auto port = std::make_unique<Port>(name, PortDirection::Output, this);
        auto* ptr = port.get();
        outputs_.push_back(std::move(port));
        return ptr;
    }

    const std::vector<std::unique_ptr<Port>>& inputs()  const { return inputs_; }
    const std::vector<std::unique_ptr<Port>>& outputs() const { return outputs_; }

    // ---- Identity ----

    uint64_t id() const { return id_; }
    void setId(uint64_t id) { id_ = id; }

    NodeGraph* graph() const { return graph_; }
    void setGraph(NodeGraph* g) { graph_ = g; }

    // Position in editor (pixel coordinates)
    float posX() const { return posX_; }
    float posY() const { return posY_; }
    void setPosition(float x, float y) { posX_ = x; posY_ = y; }

    // User-defined label override
    void setLabel(const std::string& label) { label_ = label; }
    std::string label() const { return label_.empty() ? nodeTitle() : label_; }

    // ---- Registration-based API (for simple nodes without subclassing) ----
    using ProcessFn = std::function<VariantMap(const VariantMap&)>;

    /// Set a processing function for registration-based nodes.
    void setProcessFn(ProcessFn fn) { processFn_ = std::move(fn); }

protected:
    uint64_t id_ = 0;
    NodeGraph* graph_ = nullptr;
    float posX_ = 0.0f, posY_ = 0.0f;
    std::string label_;

    std::vector<std::unique_ptr<Port>> inputs_;
    std::vector<std::unique_ptr<Port>> outputs_;
    ProcessFn processFn_;
};

/// Convenience: create a simple node via registration function.
inline std::unique_ptr<Node> createNode(
    const std::string& typeName,
    const std::string& title,
    Node::ProcessFn fn)
{
    struct FnNode final : public Node {
        std::string tn;
        std::string tl;
        FnNode(std::string tn, std::string tl) : tn(std::move(tn)), tl(std::move(tl)) {}
        std::string typeName() const override { return tn; }
        std::string nodeTitle() const override { return tl; }
        void process(const VariantMap& in, VariantMap& out) override {
            if (processFn_) out = processFn_(in);
        }
    };
    auto node = std::make_unique<FnNode>(typeName, title);
    node->setProcessFn(std::move(fn));
    return node;
}

} // namespace QBlock

#endif // QBLOCK_NODE_H