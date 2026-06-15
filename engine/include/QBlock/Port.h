#ifndef QBLOCK_PORT_H
#define QBLOCK_PORT_H

#include "DataType.h"
#include <QString>
#include <string>
#include <cstdint>

namespace QBlock {

class Node;

/// Direction of a port.
enum class PortDirection : uint8_t {
    Input,
    Output
};

/// A typed input or output port on a node.
/// Ports are the connection points for data flow and execution signals.
class Port final {
public:
    /// Construct a data port.
    Port(std::string name, DataType type, PortDirection direction, Node* parent = nullptr)
        : name_(std::move(name))
        , type_(type)
        , direction_(direction)
        , isExecFlow_(false)
        , parent_(parent)
    {}

    /// Construct an execution-flow port (signal mode).
    Port(std::string name, PortDirection direction, Node* parent = nullptr)
        : name_(std::move(name))
        , type_(DataType::ExecFlow)
        , direction_(direction)
        , isExecFlow_(true)
        , parent_(parent)
    {}

    // Accessors
    const std::string& name()        const { return name_; }
    QString            displayName() const { return QString::fromStdString(name_); }
    DataType           type()        const { return type_; }
    PortDirection      direction()   const { return direction_; }
    bool               isInput()     const { return direction_ == PortDirection::Input; }
    bool               isOutput()    const { return direction_ == PortDirection::Output; }
    bool               isExecFlow()  const { return isExecFlow_; }
    Node*              parentNode()  const { return parent_; }
    QColor             color()       const { return colorForType(type_); }
    uint64_t           id()          const { return id_; }

    void setParentNode(Node* node) { parent_ = node; }

    /// Unique identifier (set by NodeGraph on creation).
    void setId(uint64_t id) { id_ = id; }

private:
    std::string  name_;
    DataType     type_;
    PortDirection direction_;
    bool         isExecFlow_;
    Node*        parent_ = nullptr;
    uint64_t     id_ = 0;
};

} // namespace QBlock

#endif // QBLOCK_PORT_H