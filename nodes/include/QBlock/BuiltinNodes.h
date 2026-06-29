#ifndef QBLOCK_BUILTIN_NODES_H
#define QBLOCK_BUILTIN_NODES_H

#include <QBlock/Node.h>
#include <QBlock/NodeGraph.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdint>
#include <queue>
#include <stack>
#include <deque>
#include <set>
#include <unordered_set>
#include <algorithm>

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

/// Constant node holding a fixed integer value.
/// Double-click in editor to edit the value.
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

/// Constant node holding a fixed floating-point value.
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

/// Constant node holding a boolean value.
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

/// Constant node holding a string value.
class ConstantStringNode final : public Node {
public:
    ConstantStringNode() {
        addOutput("value", DataType::String);
    }
    std::string typeName() const override { return "ConstantString"; }
    std::string nodeTitle() const override { return "Const String"; }

    void setValue(const std::string& v) { value_ = v; }
    const std::string& value() const { return value_; }

    void process(const VariantMap&, VariantMap& out) override {
        out["value"] = Variant(value_);
    }

private:
    std::string value_;
};

// ============================================================================
// Variable Node - read/write storage cell
//   - Has an optional input "set" to set its value
//   - Has an output "value" that outputs the currently stored value
// ============================================================================

class VariableNode final : public Node {
public:
    VariableNode() {
        addInput("set", DataType::Generic);
        addOutput("value", DataType::Generic);
    }
    std::string typeName() const override { return "Variable"; }
    std::string nodeTitle() const override { return "Variable"; }

    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("set");
        if (it != in.end() && it->second.isValid()) {
            storedValue_ = it->second;
        }
        out["value"] = storedValue_;
    }

private:
    Variant storedValue_;
};

// ============================================================================
// Counter Node - increments on each execution
//   - Input "increment": amount to add (defaults to 1)
//   - Input "reset": resets counter to 0 when a value arrives
//   - Output "value": current counter value
// ============================================================================

class CounterNode final : public Node {
public:
    CounterNode() {
        addInput("increment", DataType::Integer);
        addInput("reset", DataType::Boolean);
        addOutput("value", DataType::Integer);
    }
    std::string typeName() const override { return "Counter"; }
    std::string nodeTitle() const override { return "Counter"; }

    void process(const VariantMap& in, VariantMap& out) override {
        // If reset port has a value that is truthy, reset counter
        auto resetIt = in.find("reset");
        if (resetIt != in.end() && resetIt->second.isValid() && resetIt->second.toBool()) {
            counter_ = 0;
        } else {
            // Otherwise increment by the increment value (or 1 by default)
            int64_t inc = 1;
            auto incIt = in.find("increment");
            if (incIt != in.end() && incIt->second.isValid()) {
                inc = incIt->second.toInt();
            }
            counter_ += inc;
        }
        out["value"] = Variant(counter_);
    }

private:
    int64_t counter_ = 0;
};

// ============================================================================
// Qt UI Design Nodes
// ============================================================================

/// Color node - stores RGBA color values for UI styling
class ColorNode final : public Node {
public:
    ColorNode() {
        addInput("r", DataType::Integer);
        addInput("g", DataType::Integer);
        addInput("b", DataType::Integer);
        addInput("a", DataType::Integer);
        addOutput("color", DataType::String);
    }
    std::string typeName() const override { return "Color"; }
    std::string nodeTitle() const override { return "Color"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int r = 0, g = 0, b = 0, a = 255;
        auto it = in.find("r"); if (it != in.end() && it->second.isValid()) r = static_cast<int>(it->second.toInt());
        it = in.find("g"); if (it != in.end() && it->second.isValid()) g = static_cast<int>(it->second.toInt());
        it = in.find("b"); if (it != in.end() && it->second.isValid()) b = static_cast<int>(it->second.toInt());
        it = in.find("a"); if (it != in.end() && it->second.isValid()) a = static_cast<int>(it->second.toInt());
        // Clamp values
        r = std::max(0, std::min(255, r));
        g = std::max(0, std::min(255, g));
        b = std::max(0, std::min(255, b));
        a = std::max(0, std::min(255, a));
        // Output as CSS color string
        char buf[32];
        if (a == 255) {
            snprintf(buf, sizeof(buf), "#%02X%02X%02X", r, g, b);
        } else {
            snprintf(buf, sizeof(buf), "rgba(%d,%d,%d,%d)", r, g, b, a);
        }
        out["color"] = Variant(std::string(buf));
    }
};

/// Qt MainWindow node - top-level window container
class QtMainWindowNode final : public Node {
public:
    QtMainWindowNode() {
        addInput("title", DataType::String);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addInput("style", DataType::String);
        addInput("widget1", DataType::Generic);
        addInput("widget2", DataType::Generic);
        addInput("widget3", DataType::Generic);
        addOutput("window", DataType::Generic);
    }
    std::string typeName() const override { return "QtMainWindow"; }
    std::string nodeTitle() const override { return "Main Window"; }

    void process(const VariantMap& in, VariantMap& out) override {
        title_ = "Qt Window";
        width_ = 800;
        height_ = 600;
        style_.clear();

        auto it = in.find("title");
        if (it != in.end() && it->second.isValid()) title_ = it->second.toString().toStdString();
        it = in.find("width");
        if (it != in.end() && it->second.isValid()) width_ = static_cast<int>(it->second.toInt());
        it = in.find("height");
        if (it != in.end() && it->second.isValid()) height_ = static_cast<int>(it->second.toInt());
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style_ = it->second.toString().toStdString();

        out["window"] = Variant(title_);
    }

private:
    std::string title_;
    int width_ = 800;
    int height_ = 600;
    std::string style_;
};

/// Qt Button node - clickable button widget
class QtButtonNode final : public Node {
public:
    QtButtonNode() {
        addInput("text", DataType::String);
        addInput("style", DataType::String);
        addInput("enabled", DataType::Boolean);
        addOutput("clicked", DataType::Boolean);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtButton"; }
    std::string nodeTitle() const override { return "Button"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string text = "Button";
        std::string style;
        bool enabled = true;

        auto it = in.find("text");
        if (it != in.end() && it->second.isValid()) text = it->second.toString().toStdString();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();
        it = in.find("enabled");
        if (it != in.end() && it->second.isValid()) enabled = it->second.toBool();

        out["clicked"] = Variant(false);
        out["widget"] = Variant(text);
    }
};

/// Qt Label node - text display widget
class QtLabelNode final : public Node {
public:
    QtLabelNode() {
        addInput("text", DataType::String);
        addInput("style", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtLabel"; }
    std::string nodeTitle() const override { return "Label"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string text = "Label";
        std::string style;

        auto it = in.find("text");
        if (it != in.end() && it->second.isValid()) text = it->second.toString().toStdString();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        out["widget"] = Variant(text);
    }
};

/// Qt LineEdit node - text input widget
class QtLineEditNode final : public Node {
public:
    QtLineEditNode() {
        addInput("placeholder", DataType::String);
        addInput("style", DataType::String);
        addInput("enabled", DataType::Boolean);
        addOutput("text", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtLineEdit"; }
    std::string nodeTitle() const override { return "Line Edit"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string placeholder;
        std::string style;
        bool enabled = true;

        auto it = in.find("placeholder");
        if (it != in.end() && it->second.isValid()) placeholder = it->second.toString().toStdString();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();
        it = in.find("enabled");
        if (it != in.end() && it->second.isValid()) enabled = it->second.toBool();

        out["text"] = Variant(std::string());
        out["widget"] = Variant(placeholder);
    }
};

/// Qt TabWidget node - tabbed container
class QtTabWidgetNode final : public Node {
public:
    QtTabWidgetNode() {
        addInput("tab1", DataType::Generic);
        addInput("tab2", DataType::Generic);
        addInput("tab3", DataType::Generic);
        addInput("style", DataType::String);
        addOutput("currentTab", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtTabWidget"; }
    std::string nodeTitle() const override { return "Tab Widget"; }

    void process(const VariantMap& in, VariantMap& out) override {
        (void)in; // Suppress unused parameter warning
        out["currentTab"] = Variant(static_cast<int64_t>(0));
        out["widget"] = Variant(std::string("TabWidget"));
    }
};

/// Qt Layout node - vertical/horizontal layout container
class QtLayoutNode final : public Node {
public:
    QtLayoutNode() {
        addInput("item1", DataType::Generic);
        addInput("item2", DataType::Generic);
        addInput("item3", DataType::Generic);
        addInput("direction", DataType::String);
        addOutput("layout", DataType::Generic);
    }
    std::string typeName() const override { return "QtLayout"; }
    std::string nodeTitle() const override { return "Layout"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string dir = "vertical";
        auto it = in.find("direction");
        if (it != in.end() && it->second.isValid()) dir = it->second.toString().toStdString();
        out["layout"] = Variant(dir);
    }
};

/// Qt Slider node - value slider widget
class QtSliderNode final : public Node {
public:
    QtSliderNode() {
        addInput("min", DataType::Integer);
        addInput("max", DataType::Integer);
        addInput("value", DataType::Integer);
        addInput("style", DataType::String);
        addOutput("value", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtSlider"; }
    std::string nodeTitle() const override { return "Slider"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t minVal = 0, maxVal = 100, val = 50;

        auto it = in.find("min");
        if (it != in.end() && it->second.isValid()) minVal = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();
        it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();

        // Clamp value
        val = std::max(minVal, std::min(maxVal, val));
        out["value"] = Variant(val);
        out["widget"] = Variant(std::string("Slider"));
    }
};

/// Qt CheckBox node - checkbox widget
class QtCheckBoxNode final : public Node {
public:
    QtCheckBoxNode() {
        addInput("text", DataType::String);
        addInput("checked", DataType::Boolean);
        addInput("style", DataType::String);
        addOutput("checked", DataType::Boolean);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtCheckBox"; }
    std::string nodeTitle() const override { return "Check Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string text = "CheckBox";
        bool checked = false;

        auto it = in.find("text");
        if (it != in.end() && it->second.isValid()) text = it->second.toString().toStdString();
        it = in.find("checked");
        if (it != in.end() && it->second.isValid()) checked = it->second.toBool();

        out["checked"] = Variant(checked);
        out["widget"] = Variant(text);
    }
};

/// Qt ComboBox node - dropdown selection widget
class QtComboBoxNode final : public Node {
public:
    QtComboBoxNode() {
        addInput("items", DataType::String);
        addInput("style", DataType::String);
        addOutput("currentText", DataType::String);
        addOutput("currentIndex", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtComboBox"; }
    std::string nodeTitle() const override { return "Combo Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string items = "Item1;Item2;Item3";

        auto it = in.find("items");
        if (it != in.end() && it->second.isValid()) items = it->second.toString().toStdString();

        out["currentText"] = Variant(std::string("Item1"));
        out["currentIndex"] = Variant(static_cast<int64_t>(0));
        out["widget"] = Variant(items);
    }
};

/// Qt SpinBox node - number input with arrows
class QtSpinBoxNode final : public Node {
public:
    QtSpinBoxNode() {
        addInput("min", DataType::Integer);
        addInput("max", DataType::Integer);
        addInput("value", DataType::Integer);
        addInput("style", DataType::String);
        addOutput("value", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtSpinBox"; }
    std::string nodeTitle() const override { return "Spin Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t minVal = 0, maxVal = 100, val = 0;

        auto it = in.find("min");
        if (it != in.end() && it->second.isValid()) minVal = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();
        it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();

        val = std::max(minVal, std::min(maxVal, val));
        out["value"] = Variant(val);
        out["widget"] = Variant(std::string("SpinBox"));
    }
};

/// Qt ProgressBar node - progress indicator
class QtProgressBarNode final : public Node {
public:
    QtProgressBarNode() {
        addInput("value", DataType::Integer);
        addInput("max", DataType::Integer);
        addInput("style", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "QtProgressBar"; }
    std::string nodeTitle() const override { return "Progress Bar"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t val = 0, maxVal = 100;

        auto it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();

        val = std::max(int64_t(0), std::min(maxVal, val));
        out["widget"] = Variant(std::string("ProgressBar"));
    }
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
            std::cout << "[QBlock Print] " << it->second.toString().toStdString() << std::endl;
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
// Container Nodes (std containers)
// ============================================================================

class StdVectorNode final : public Node {
public:
    StdVectorNode() {
        addInput("size", DataType::Integer);
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("element", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdVector"; }
    std::string nodeTitle() const override { return "列表"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto sizeIt = in.find("size");
        if (sizeIt != in.end() && sizeIt->second.isValid()) {
            int64_t newSize = sizeIt->second.toInt();
            if (newSize >= 0) {
                values_.resize(static_cast<size_t>(newSize));
            }
        }
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            if (values_.empty()) {
                values_.push_back(initIt->second);
            } else {
                values_[0] = initIt->second;
            }
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(values_.size()));
        if (!values_.empty()) {
            out["element"] = values_[0];
        }
    }
    std::vector<Variant>& values() { return values_; }
private:
    std::vector<Variant> values_;
};

class StdVectorPushNode final : public Node {
public:
    StdVectorPushNode() {
        addInput("container", DataType::Generic);
        addInput("value1", DataType::Generic);
        addInput("value2", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdVectorPush"; }
    std::string nodeTitle() const override { return "添加元素"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* vecNode = reinterpret_cast<StdVectorNode*>(ptr);
            if (vecNode) {
                auto val1It = in.find("value1");
                if (val1It != in.end() && val1It->second.isValid()) {
                    vecNode->values().push_back(val1It->second);
                }
                auto val2It = in.find("value2");
                if (val2It != in.end() && val2It->second.isValid()) {
                    vecNode->values().push_back(val2It->second);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(vecNode->values().size()));
            }
        }
    }
};

class StdQueueNode final : public Node {
public:
    StdQueueNode() {
        addInput("size", DataType::Integer);
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("front", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdQueue"; }
    std::string nodeTitle() const override { return "队列"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto sizeIt = in.find("size");
        if (sizeIt != in.end() && sizeIt->second.isValid()) {
            int64_t newSize = sizeIt->second.toInt();
            while (queue_.size() > static_cast<size_t>(newSize) && !queue_.empty()) {
                queue_.pop();
            }
        }
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            queue_.push(initIt->second);
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(queue_.size()));
        if (!queue_.empty()) {
            out["front"] = queue_.front();
        }
    }
    std::queue<Variant>& queue() { return queue_; }
private:
    std::queue<Variant> queue_;
};

class StdQueuePushNode final : public Node {
public:
    StdQueuePushNode() {
        addInput("container", DataType::Generic);
        addInput("value", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdQueuePush"; }
    std::string nodeTitle() const override { return "入队"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* queueNode = reinterpret_cast<StdQueueNode*>(ptr);
            if (queueNode) {
                auto valIt = in.find("value");
                if (valIt != in.end() && valIt->second.isValid()) {
                    queueNode->queue().push(valIt->second);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(queueNode->queue().size()));
            }
        }
    }
};

class StdStackNode final : public Node {
public:
    StdStackNode() {
        addInput("size", DataType::Integer);
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("top", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdStack"; }
    std::string nodeTitle() const override { return "栈"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto sizeIt = in.find("size");
        if (sizeIt != in.end() && sizeIt->second.isValid()) {
            int64_t newSize = sizeIt->second.toInt();
            while (stack_.size() > static_cast<size_t>(newSize) && !stack_.empty()) {
                stack_.pop();
            }
        }
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            stack_.push(initIt->second);
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(stack_.size()));
        if (!stack_.empty()) {
            out["top"] = stack_.top();
        }
    }
    std::stack<Variant>& stack() { return stack_; }
private:
    std::stack<Variant> stack_;
};

class StdStackPushNode final : public Node {
public:
    StdStackPushNode() {
        addInput("container", DataType::Generic);
        addInput("value", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdStackPush"; }
    std::string nodeTitle() const override { return "压栈"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* stackNode = reinterpret_cast<StdStackNode*>(ptr);
            if (stackNode) {
                auto valIt = in.find("value");
                if (valIt != in.end() && valIt->second.isValid()) {
                    stackNode->stack().push(valIt->second);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(stackNode->stack().size()));
            }
        }
    }
};

class StdDequeNode final : public Node {
public:
    StdDequeNode() {
        addInput("size", DataType::Integer);
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("front", DataType::Generic);
        addOutput("back", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdDeque"; }
    std::string nodeTitle() const override { return "双端队列"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto sizeIt = in.find("size");
        if (sizeIt != in.end() && sizeIt->second.isValid()) {
            int64_t newSize = sizeIt->second.toInt();
            while (deque_.size() > static_cast<size_t>(newSize) && !deque_.empty()) {
                deque_.pop_back();
            }
        }
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            deque_.push_back(initIt->second);
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(deque_.size()));
        if (!deque_.empty()) {
            out["front"] = deque_.front();
            out["back"] = deque_.back();
        }
    }
    std::deque<Variant>& deque() { return deque_; }
private:
    std::deque<Variant> deque_;
};

class StdDequePushNode final : public Node {
public:
    StdDequePushNode() {
        addInput("container", DataType::Generic);
        addInput("value", DataType::Generic);
        addInput("front", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
    }
    std::string typeName() const override { return "StdDequePush"; }
    std::string nodeTitle() const override { return "双端入队"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* dequeNode = reinterpret_cast<StdDequeNode*>(ptr);
            if (dequeNode) {
                auto frontIt = in.find("front");
                if (frontIt != in.end() && frontIt->second.isValid()) {
                    dequeNode->deque().push_front(frontIt->second);
                }
                auto valIt = in.find("value");
                if (valIt != in.end() && valIt->second.isValid()) {
                    dequeNode->deque().push_back(valIt->second);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(dequeNode->deque().size()));
            }
        }
    }
};

class StdSetNode final : public Node {
public:
    StdSetNode() {
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
        addOutput("contains", DataType::Boolean);
    }
    std::string typeName() const override { return "StdSet"; }
    std::string nodeTitle() const override { return "集合"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            set_.insert(initIt->second.toString().toStdString());
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(set_.size()));
        out["contains"] = Variant(set_.size() > 0);
    }
    std::set<std::string>& set() { return set_; }
private:
    std::set<std::string> set_;
};

class StdSetInsertNode final : public Node {
public:
    StdSetInsertNode() {
        addInput("container", DataType::Generic);
        addInput("value", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
        addOutput("contains", DataType::Boolean);
    }
    std::string typeName() const override { return "StdSetInsert"; }
    std::string nodeTitle() const override { return "插入集合"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* setNode = reinterpret_cast<StdSetNode*>(ptr);
            if (setNode) {
                auto valIt = in.find("value");
                bool contained = false;
                if (valIt != in.end() && valIt->second.isValid()) {
                    std::string strVal = valIt->second.toString().toStdString();
                    contained = setNode->set().count(strVal) > 0;
                    setNode->set().insert(strVal);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(setNode->set().size()));
                out["contains"] = Variant(contained);
            }
        }
    }
};

class StdUnorderedSetNode final : public Node {
public:
    StdUnorderedSetNode() {
        addInput("init", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
        addOutput("contains", DataType::Boolean);
    }
    std::string typeName() const override { return "StdUnorderedSet"; }
    std::string nodeTitle() const override { return "无序集合"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto initIt = in.find("init");
        if (initIt != in.end() && initIt->second.isValid()) {
            set_.insert(initIt->second.toString().toStdString());
        }
        out["container"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["length"] = Variant(static_cast<int64_t>(set_.size()));
        out["contains"] = Variant(set_.size() > 0);
    }
    std::unordered_set<std::string>& set() { return set_; }
private:
    std::unordered_set<std::string> set_;
};

class StdUnorderedSetInsertNode final : public Node {
public:
    StdUnorderedSetInsertNode() {
        addInput("container", DataType::Generic);
        addInput("value", DataType::Generic);
        addOutput("container", DataType::Generic);
        addOutput("length", DataType::Integer);
        addOutput("contains", DataType::Boolean);
    }
    std::string typeName() const override { return "StdUnorderedSetInsert"; }
    std::string nodeTitle() const override { return "插入无序集"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto contIt = in.find("container");
        if (contIt != in.end() && contIt->second.isValid()) {
            intptr_t ptr = static_cast<intptr_t>(contIt->second.toInt());
            auto* setNode = reinterpret_cast<StdUnorderedSetNode*>(ptr);
            if (setNode) {
                auto valIt = in.find("value");
                bool contained = false;
                if (valIt != in.end() && valIt->second.isValid()) {
                    std::string strVal = valIt->second.toString().toStdString();
                    contained = setNode->set().count(strVal) > 0;
                    setNode->set().insert(strVal);
                }
                out["container"] = contIt->second;
                out["length"] = Variant(static_cast<int64_t>(setNode->set().size()));
                out["contains"] = Variant(contained);
            }
        }
    }
};

class StdPairNode final : public Node {
public:
    StdPairNode() {
        addInput("first", DataType::Generic);
        addInput("second", DataType::Generic);
        addOutput("pair", DataType::Generic);
        addOutput("first", DataType::Generic);
        addOutput("second", DataType::Generic);
    }
    std::string typeName() const override { return "StdPair"; }
    std::string nodeTitle() const override { return "配对"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto firstIt = in.find("first");
        auto secondIt = in.find("second");
        if (firstIt != in.end() && firstIt->second.isValid()) {
            first_ = firstIt->second;
        }
        if (secondIt != in.end() && secondIt->second.isValid()) {
            second_ = secondIt->second;
        }
        out["pair"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["first"] = first_;
        out["second"] = second_;
    }
private:
    Variant first_;
    Variant second_;
};

class StructNode final : public Node {
public:
    StructNode() {
        addInput("field1", DataType::Generic);
        addInput("field2", DataType::Generic);
        addInput("field3", DataType::Generic);
        addOutput("struct", DataType::Generic);
        addOutput("field1", DataType::Generic);
        addOutput("field2", DataType::Generic);
        addOutput("field3", DataType::Generic);
    }
    std::string typeName() const override { return "Struct"; }
    std::string nodeTitle() const override { return "结构体"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("field1");
        if (it != in.end() && it->second.isValid()) field1_ = it->second;
        it = in.find("field2");
        if (it != in.end() && it->second.isValid()) field2_ = it->second;
        it = in.find("field3");
        if (it != in.end() && it->second.isValid()) field3_ = it->second;
        out["struct"] = Variant(static_cast<int64_t>(reinterpret_cast<intptr_t>(this)));
        out["field1"] = field1_;
        out["field2"] = field2_;
        out["field3"] = field3_;
    }
private:
    Variant field1_, field2_, field3_;
};

class VoidNode final : public Node {
public:
    VoidNode() {
        addInput("in", DataType::Generic);
    }
    std::string typeName() const override { return "Void"; }
    std::string nodeTitle() const override { return "void"; }
    void process(const VariantMap&, VariantMap&) override {}
};

class SortNode final : public Node {
public:
    SortNode() {
        addInput("value", DataType::Generic);
        addInput("ascending", DataType::Boolean);
        addOutput("sorted", DataType::Generic);
        addOutput("size", DataType::Integer);
    }
    std::string typeName() const override { return "Sort"; }
    std::string nodeTitle() const override { return "sort"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto valIt = in.find("value");
        if (valIt != in.end() && valIt->second.isValid()) {
            values_.push_back(valIt->second);
        }
        bool ascending = true;
        auto ascIt = in.find("ascending");
        if (ascIt != in.end() && ascIt->second.isValid()) {
            ascending = ascIt->second.toBool();
        }
        if (ascending) {
            std::sort(values_.begin(), values_.end(), [](const Variant& a, const Variant& b) {
                return a.toString() < b.toString();
            });
        } else {
            std::sort(values_.begin(), values_.end(), [](const Variant& a, const Variant& b) {
                return a.toString() > b.toString();
            });
        }
        out["size"] = Variant(static_cast<int64_t>(values_.size()));
        if (!values_.empty()) {
            out["sorted"] = values_.front();
        }
    }
private:
    std::vector<Variant> values_;
};

// ============================================================================
// Entry/Event Nodes
// ============================================================================

class InputBlockNode final : public Node {
public:
    InputBlockNode() {
        addExecOutput("exec");
        addOutput("value", DataType::Integer);
    }
    std::string typeName() const override { return "InputBlock"; }
    void process(const VariantMap& in, VariantMap& out) override {
        out["exec"] = Variant(true);
        out["value"] = Variant(int64_t(0));
    }
};

class EventBlockNode final : public Node {
public:
    EventBlockNode() {
        addInput("condition", DataType::Boolean);
        addExecOutput("exec");
    }
    std::string typeName() const override { return "EventBlock"; }
    void process(const VariantMap& in, VariantMap& out) override {
        if (in.at("condition").toBool()) {
            out["exec"] = Variant(true);
        }
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
    if (typeName == "Variable")         return std::make_unique<VariableNode>();
    if (typeName == "Counter")          return std::make_unique<CounterNode>();
    // Qt UI nodes
    if (typeName == "Color")            return std::make_unique<ColorNode>();
    if (typeName == "QtMainWindow")     return std::make_unique<QtMainWindowNode>();
    if (typeName == "QtButton")         return std::make_unique<QtButtonNode>();
    if (typeName == "QtLabel")          return std::make_unique<QtLabelNode>();
    if (typeName == "QtLineEdit")       return std::make_unique<QtLineEditNode>();
    if (typeName == "QtTabWidget")      return std::make_unique<QtTabWidgetNode>();
    if (typeName == "QtLayout")         return std::make_unique<QtLayoutNode>();
    if (typeName == "QtSlider")         return std::make_unique<QtSliderNode>();
    if (typeName == "QtCheckBox")       return std::make_unique<QtCheckBoxNode>();
    if (typeName == "QtComboBox")       return std::make_unique<QtComboBoxNode>();
    if (typeName == "QtSpinBox")        return std::make_unique<QtSpinBoxNode>();
    if (typeName == "QtProgressBar")    return std::make_unique<QtProgressBarNode>();
    // Container nodes
    if (typeName == "StdVector")         return std::make_unique<StdVectorNode>();
    if (typeName == "StdVectorPush")     return std::make_unique<StdVectorPushNode>();
    if (typeName == "StdQueue")          return std::make_unique<StdQueueNode>();
    if (typeName == "StdQueuePush")      return std::make_unique<StdQueuePushNode>();
    if (typeName == "StdStack")          return std::make_unique<StdStackNode>();
    if (typeName == "StdStackPush")      return std::make_unique<StdStackPushNode>();
    if (typeName == "StdDeque")          return std::make_unique<StdDequeNode>();
    if (typeName == "StdDequePush")      return std::make_unique<StdDequePushNode>();
    if (typeName == "StdSet")            return std::make_unique<StdSetNode>();
    if (typeName == "StdSetInsert")      return std::make_unique<StdSetInsertNode>();
    if (typeName == "StdUnorderedSet")   return std::make_unique<StdUnorderedSetNode>();
    if (typeName == "StdUnorderedSetInsert") return std::make_unique<StdUnorderedSetInsertNode>();
    if (typeName == "StdPair")           return std::make_unique<StdPairNode>();
    if (typeName == "Struct")            return std::make_unique<StructNode>();
    if (typeName == "Void")              return std::make_unique<VoidNode>();
    if (typeName == "Sort")              return std::make_unique<SortNode>();
    // Entry/Event nodes
    if (typeName == "InputBlock")       return std::make_unique<InputBlockNode>();
    if (typeName == "EventBlock")       return std::make_unique<EventBlockNode>();
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