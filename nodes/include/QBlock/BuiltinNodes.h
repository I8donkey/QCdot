#ifndef QBLOCK_BUILTIN_NODES_H
#define QBLOCK_BUILTIN_NODES_H

#include <QBlock/Node.h>
#include <QBlock/NodeGraph.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#endif

namespace QBlock {
namespace Builtin {

// ============================================================================
// Math Nodes
// ============================================================================

class AddNode final : public Node {
public:
    AddNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Add"; }
    std::string nodeTitle() const override { return "Add"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toFloat() + in.at("b").toFloat());
    }
};

class SubtractNode final : public Node {
public:
    SubtractNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Subtract"; }
    std::string nodeTitle() const override { return "Subtract"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toFloat() - in.at("b").toFloat());
    }
};

class MultiplyNode final : public Node {
public:
    MultiplyNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Multiply"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toFloat() * in.at("b").toFloat());
    }
};

class DivideNode final : public Node {
public:
    DivideNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Divide"; }
    void process(const VariantMap& in, VariantMap& out) override {
        double b = in.at("b").toFloat();
        out["result"] = Variant(b != 0.0 ? in.at("a").toFloat() / b : 0.0);
    }
};

class ModuloNode final : public Node {
public:
    ModuloNode() {
        addInput("a", DataType::Integer);
        addInput("b", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "Modulo"; }
    void process(const VariantMap& in, VariantMap& out) override {
        int64_t b = in.at("b").toInt();
        out["result"] = Variant(b != 0 ? in.at("a").toInt() % b : 0);
    }
};

class PowerNode final : public Node {
public:
    PowerNode() {
        addInput("base", DataType::Float);
        addInput("exp", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Power"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(std::pow(in.at("base").toFloat(), in.at("exp").toFloat()));
    }
};

class SqrtNode final : public Node {
public:
    SqrtNode() {
        addInput("value", DataType::Float);
        addOutput("result", DataType::Float);
    }
    std::string typeName() const override { return "Sqrt"; }
    void process(const VariantMap& in, VariantMap& out) override {
        double v = in.at("value").toFloat();
        out["result"] = Variant(v >= 0 ? std::sqrt(v) : 0.0);
    }
};

// ============================================================================
// Comparison Nodes
// ============================================================================

class EqualNode final : public Node {
public:
    EqualNode() {
        addInput("a", DataType::Generic);
        addInput("b", DataType::Generic);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "Equal"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a") == in.at("b"));
    }
};

class GreaterThanNode final : public Node {
public:
    GreaterThanNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "GreaterThan"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toFloat() > in.at("b").toFloat());
    }
};

class LessThanNode final : public Node {
public:
    LessThanNode() {
        addInput("a", DataType::Float);
        addInput("b", DataType::Float);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "LessThan"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toFloat() < in.at("b").toFloat());
    }
};

// ============================================================================
// Logic Nodes
// ============================================================================

class AndNode final : public Node {
public:
    AndNode() {
        addInput("a", DataType::Boolean);
        addInput("b", DataType::Boolean);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "And"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toBool() && in.at("b").toBool());
    }
};

class OrNode final : public Node {
public:
    OrNode() {
        addInput("a", DataType::Boolean);
        addInput("b", DataType::Boolean);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "Or"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toBool() || in.at("b").toBool());
    }
};

class NotNode final : public Node {
public:
    NotNode() {
        addInput("a", DataType::Boolean);
        addOutput("result", DataType::Boolean);
    }
    std::string typeName() const override { return "Not"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(!in.at("a").toBool());
    }
};

// ============================================================================
// Control Flow Nodes
// ============================================================================

class IfElseNode final : public Node {
public:
    IfElseNode() {
        addInput("condition", DataType::Boolean);
        addInput("trueValue", DataType::Generic);
        addInput("falseValue", DataType::Generic);
        addOutput("result", DataType::Generic);
    }
    std::string typeName() const override { return "IfElse"; }
    std::string nodeTitle() const override { return "If/Else"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = in.at("condition").toBool() ? in.at("trueValue") : in.at("falseValue");
    }
};

/// Signal-mode branch node
class IfElseBranchNode final : public Node {
public:
    IfElseBranchNode() {
        addExecInput("exec");
        addInput("condition", DataType::Boolean);
        addExecOutput("true");
        addExecOutput("false");
    }
    std::string typeName() const override { return "IfElseBranch"; }
    std::string nodeTitle() const override { return "If/Else Branch"; }
    void process(const VariantMap& in, VariantMap&) override {
        // Signal mode: execution routing based on condition
        (void)in;
    }
};

// ============================================================================
// Variable / Constant Nodes
// ============================================================================

class ConstantIntNode final : public Node {
public:
    ConstantIntNode() {
        addOutput("value", DataType::Integer);
    }
    std::string typeName() const override { return "ConstantInt"; }
    std::string nodeTitle() const override { return "Const Int"; }

    void setValue(int64_t v) { value_ = v; }
    int64_t value() const { return value_; }

    void process(const VariantMap&, VariantMap& out) override {
        out["value"] = Variant(value_);
    }

private:
    int64_t value_ = 0;
};

class ConstantFloatNode final : public Node {
public:
    ConstantFloatNode() {
        addOutput("value", DataType::Float);
    }
    std::string typeName() const override { return "ConstantFloat"; }
    std::string nodeTitle() const override { return "Const Float"; }

    void setValue(double v) { value_ = v; }
    double value() const { return value_; }

    void process(const VariantMap&, VariantMap& out) override {
        out["value"] = Variant(value_);
    }

private:
    double value_ = 0.0;
};

class ConstantBoolNode final : public Node {
public:
    ConstantBoolNode() {
        addOutput("value", DataType::Boolean);
    }
    std::string typeName() const override { return "ConstantBool"; }
    std::string nodeTitle() const override { return "Const Bool"; }

    void setValue(bool v) { value_ = v; }
    bool value() const { return value_; }

    void process(const VariantMap&, VariantMap& out) override {
        out["value"] = Variant(value_);
    }

private:
    bool value_ = false;
};

class ConstantStringNode final : public Node {
public:
    ConstantStringNode() {
        addOutput("value", DataType::String);
    }
    std::string typeName() const override { return "ConstantString"; }
    std::string nodeTitle() const override { return "Const String"; }

    void setValue(const std::string& v) { value_ = v; }
    std::string value() const { return value_; }

    void process(const VariantMap&, VariantMap& out) override {
        out["value"] = Variant(value_);
    }

private:
    std::string value_;
};

// ============================================================================
// Type Conversion Nodes
// ============================================================================

class IntToFloatNode final : public Node {
public:
    IntToFloatNode() {
        addInput("in", DataType::Integer);
        addOutput("out", DataType::Float);
    }
    std::string typeName() const override { return "IntToFloat"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["out"] = Variant(static_cast<double>(in.at("in").toInt()));
    }
};

class FloatToIntNode final : public Node {
public:
    FloatToIntNode() {
        addInput("in", DataType::Float);
        addOutput("out", DataType::Integer);
    }
    std::string typeName() const override { return "FloatToInt"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["out"] = Variant(static_cast<int64_t>(in.at("in").toFloat()));
    }
};

class ToStringNode final : public Node {
public:
    ToStringNode() {
        addInput("in", DataType::Generic);
        addOutput("out", DataType::String);
    }
    std::string typeName() const override { return "ToString"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["out"] = Variant(in.at("in").toString());
    }
};

// ============================================================================
// String Operation Nodes
// ============================================================================

class StringConcatNode final : public Node {
public:
    StringConcatNode() {
        addInput("a", DataType::String);
        addInput("b", DataType::String);
        addOutput("result", DataType::String);
    }
    std::string typeName() const override { return "StringConcat"; }
    std::string nodeTitle() const override { return "Concat"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toString() + in.at("b").toString());
    }
};

class StringLengthNode final : public Node {
public:
    StringLengthNode() {
        addInput("str", DataType::String);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StringLength"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["length"] = Variant(static_cast<int64_t>(in.at("str").toString().length()));
    }
};

class StringSubstringNode final : public Node {
public:
    StringSubstringNode() {
        addInput("str", DataType::String);
        addInput("start", DataType::Integer);
        addInput("length", DataType::Integer);
        addOutput("result", DataType::String);
    }
    std::string typeName() const override { return "StringSubstring"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto s = in.at("str").toString();
        int start = static_cast<int>(in.at("start").toInt());
        int len = static_cast<int>(in.at("length").toInt());
        out["result"] = Variant(s.mid(start, len));
    }
};

// ============================================================================
// I/O Nodes
// ============================================================================

class PrintNode final : public Node {
public:
    PrintNode() {
        addInput("value", DataType::Generic);
        addOutput("out", DataType::Generic);  // passthrough
    }
    std::string typeName() const override { return "Print"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("value");
        if (it != in.end()) {
            qDebug().noquote() << "[QBlock Print]" << it->second.toString();
            out["out"] = it->second;
        }
    }
};

// ============================================================================
// Binary Operation Nodes
// ============================================================================

class BinaryAndNode final : public Node {
public:
    BinaryAndNode() {
        addInput("a", DataType::Integer);
        addInput("b", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "BinaryAnd"; }
    std::string nodeTitle() const override { return "Bitwise AND"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toInt() & in.at("b").toInt());
    }
};

class BinaryOrNode final : public Node {
public:
    BinaryOrNode() {
        addInput("a", DataType::Integer);
        addInput("b", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "BinaryOr"; }
    std::string nodeTitle() const override { return "Bitwise OR"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toInt() | in.at("b").toInt());
    }
};

class BinaryXorNode final : public Node {
public:
    BinaryXorNode() {
        addInput("a", DataType::Integer);
        addInput("b", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "BinaryXor"; }
    std::string nodeTitle() const override { return "Bitwise XOR"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("a").toInt() ^ in.at("b").toInt());
    }
};

class BinaryShiftLeftNode final : public Node {
public:
    BinaryShiftLeftNode() {
        addInput("value", DataType::Integer);
        addInput("bits", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "ShiftLeft"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("value").toInt() << in.at("bits").toInt());
    }
};

class BinaryShiftRightNode final : public Node {
public:
    BinaryShiftRightNode() {
        addInput("value", DataType::Integer);
        addInput("bits", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "ShiftRight"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["result"] = Variant(in.at("value").toInt() >> in.at("bits").toInt());
    }
};

// ============================================================================
// File I/O Nodes
// ============================================================================

class FileReadTextNode final : public Node {
public:
    FileReadTextNode() {
        addInput("path", DataType::FilePath);
        addOutput("content", DataType::String);
        addOutput("success", DataType::Boolean);
    }
    std::string typeName() const override { return "FileReadText"; }
    std::string nodeTitle() const override { return "Read Text File"; }
    void process(const VariantMap& in, VariantMap& out) override {
        std::string path = in.at("path").toString().toStdString();
        std::ifstream file(path);
        if (file) {
            std::stringstream ss;
            ss << file.rdbuf();
            out["content"] = Variant(ss.str());
            out["success"] = Variant(true);
        } else {
            out["content"] = Variant(std::string());
            out["success"] = Variant(false);
        }
    }
};

class FileWriteTextNode final : public Node {
public:
    FileWriteTextNode() {
        addInput("path", DataType::FilePath);
        addInput("content", DataType::String);
        addOutput("success", DataType::Boolean);
    }
    std::string typeName() const override { return "FileWriteText"; }
    std::string nodeTitle() const override { return "Write Text File"; }
    void process(const VariantMap& in, VariantMap& out) override {
        std::string path = in.at("path").toString().toStdString();
        std::string content = in.at("content").toString().toStdString();
        std::ofstream file(path);
        if (file) {
            file << content;
            out["success"] = Variant(true);
        } else {
            out["success"] = Variant(false);
        }
    }
};

class FileReadBinaryNode final : public Node {
public:
    FileReadBinaryNode() {
        addInput("path", DataType::FilePath);
        addOutput("data", DataType::Binary);
        addOutput("success", DataType::Boolean);
    }
    std::string typeName() const override { return "FileReadBinary"; }
    std::string nodeTitle() const override { return "Read Binary File"; }
    void process(const VariantMap& in, VariantMap& out) override {
        std::string path = in.at("path").toString().toStdString();
        std::ifstream file(path, std::ios::binary);
        if (file) {
            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<uint8_t> buf(static_cast<size_t>(size));
            if (file.read(reinterpret_cast<char*>(buf.data()), size)) {
                out["data"] = Variant(std::move(buf));
                out["success"] = Variant(true);
            } else {
                out["success"] = Variant(false);
            }
        } else {
            out["success"] = Variant(false);
        }
    }
};

// ============================================================================
// C++ Std Utility Nodes
// ============================================================================

class RandomIntNode final : public Node {
public:
    RandomIntNode() {
        addInput("min", DataType::Integer);
        addInput("max", DataType::Integer);
        addOutput("result", DataType::Integer);
    }
    std::string typeName() const override { return "RandomInt"; }
    std::string nodeTitle() const override { return "Random Int"; }
    void process(const VariantMap& in, VariantMap& out) override {
        int64_t min = in.at("min").toInt();
        int64_t max = in.at("max").toInt();
        if (max < min) std::swap(min, max);
        out["result"] = Variant(min + static_cast<int64_t>(std::rand() % (max - min + 1)));
    }
};

class SleepNode final : public Node {
public:
    SleepNode() {
        addInput("ms", DataType::Integer);
        addOutput("done", DataType::Boolean);
    }
    std::string typeName() const override { return "Sleep"; }
    void process(const VariantMap& in, VariantMap& out) override {
#ifdef _WIN32
        Sleep(static_cast<DWORD>(in.at("ms").toInt()));
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(in.at("ms").toInt()));
#endif
        out["done"] = Variant(true);
    }
};

// ============================================================================
// Factory: create any builtin node by type name
// ============================================================================

inline std::unique_ptr<Node> createBuiltinNode(const std::string& typeName) {
    if (typeName == "Add")              return std::make_unique<AddNode>();
    if (typeName == "Subtract")         return std::make_unique<SubtractNode>();
    if (typeName == "Multiply")         return std::make_unique<MultiplyNode>();
    if (typeName == "Divide")           return std::make_unique<DivideNode>();
    if (typeName == "Modulo")           return std::make_unique<ModuloNode>();
    if (typeName == "Power")            return std::make_unique<PowerNode>();
    if (typeName == "Sqrt")             return std::make_unique<SqrtNode>();
    if (typeName == "Equal")            return std::make_unique<EqualNode>();
    if (typeName == "GreaterThan")      return std::make_unique<GreaterThanNode>();
    if (typeName == "LessThan")         return std::make_unique<LessThanNode>();
    if (typeName == "And")              return std::make_unique<AndNode>();
    if (typeName == "Or")               return std::make_unique<OrNode>();
    if (typeName == "Not")              return std::make_unique<NotNode>();
    if (typeName == "IfElse")           return std::make_unique<IfElseNode>();
    if (typeName == "IfElseBranch")     return std::make_unique<IfElseBranchNode>();
    if (typeName == "ConstantInt")      return std::make_unique<ConstantIntNode>();
    if (typeName == "ConstantFloat")    return std::make_unique<ConstantFloatNode>();
    if (typeName == "ConstantBool")     return std::make_unique<ConstantBoolNode>();
    if (typeName == "ConstantString")   return std::make_unique<ConstantStringNode>();
    if (typeName == "IntToFloat")       return std::make_unique<IntToFloatNode>();
    if (typeName == "FloatToInt")       return std::make_unique<FloatToIntNode>();
    if (typeName == "ToString")         return std::make_unique<ToStringNode>();
    if (typeName == "StringConcat")     return std::make_unique<StringConcatNode>();
    if (typeName == "StringLength")     return std::make_unique<StringLengthNode>();
    if (typeName == "StringSubstring")  return std::make_unique<StringSubstringNode>();
    if (typeName == "Print")            return std::make_unique<PrintNode>();
    if (typeName == "BinaryAnd")        return std::make_unique<BinaryAndNode>();
    if (typeName == "BinaryOr")         return std::make_unique<BinaryOrNode>();
    if (typeName == "BinaryXor")        return std::make_unique<BinaryXorNode>();
    if (typeName == "ShiftLeft")        return std::make_unique<BinaryShiftLeftNode>();
    if (typeName == "ShiftRight")       return std::make_unique<BinaryShiftRightNode>();
    if (typeName == "FileReadText")     return std::make_unique<FileReadTextNode>();
    if (typeName == "FileWriteText")    return std::make_unique<FileWriteTextNode>();
    if (typeName == "FileReadBinary")   return std::make_unique<FileReadBinaryNode>();
    if (typeName == "RandomInt")        return std::make_unique<RandomIntNode>();
    if (typeName == "Sleep")            return std::make_unique<SleepNode>();
    return nullptr;
}

/// Register all builtin nodes into a node graph for display/use.
inline void registerBuiltinNodes(const std::string& category, NodeGraph& graph) {
    (void)category;
    (void)graph;
    // In the future, this can register node type metadata for the editor palette.
}

} // namespace Builtin
} // namespace QBlock

#endif // QBLOCK_BUILTIN_NODES_H