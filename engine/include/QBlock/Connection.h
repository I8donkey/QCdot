#ifndef QBLOCK_CONNECTION_H
#define QBLOCK_CONNECTION_H

#include <cstdint>

namespace QBlock {

class Port;

/// A connection between an output port and an input port.
class Connection final {
public:
    Connection(Port* source, Port* target, uint64_t id = 0)
        : source_(source), target_(target), id_(id)
    {}

    Port* source() const { return source_; }
    Port* target() const { return target_; }
    uint64_t id() const { return id_; }
    void setId(uint64_t id) { id_ = id; }

private:
    Port* source_ = nullptr;
    Port* target_ = nullptr;
    uint64_t id_ = 0;
};

} // namespace QBlock

#endif // QBLOCK_CONNECTION_H