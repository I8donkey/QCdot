#ifndef QBLOCK_BUILTIN_NODES_H
#define QBLOCK_BUILTIN_NODES_H

#include <QBlock/Node.h>
#include <QBlock/NodeGraph.h>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QProgressBar>
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
    ~QtMainWindowNode() {
        if (window_) {
            window_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtMainWindow"; }
    std::string nodeTitle() const override { return "Main Window"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string title = "Qt Window";
        int width = 800;
        int height = 600;
        std::string style;

        auto it = in.find("title");
        if (it != in.end() && it->second.isValid()) title = it->second.toString().toStdString();
        it = in.find("width");
        if (it != in.end() && it->second.isValid()) width = static_cast<int>(it->second.toInt());
        it = in.find("height");
        if (it != in.end() && it->second.isValid()) height = static_cast<int>(it->second.toInt());
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        // Create or update window
        if (!window_) {
            window_ = new QMainWindow();
        }
        window_->setWindowTitle(QString::fromStdString(title));
        window_->resize(width, height);
        if (!style.empty()) {
            window_->setStyleSheet(QString::fromStdString(style));
        }

        // Collect child widgets from all widgetN inputs
        QWidget* centralWidget = new QWidget(window_);
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        layout->setSpacing(8);
        layout->setContentsMargins(12, 12, 12, 12);

        for (const auto& port : inputs()) {
            const std::string& name = port->name();
            if (name.find("widget") == 0) {
                auto wit = in.find(name);
                if (wit != in.end() && wit->second.isValid() && wit->second.type() == DataType::Pointer) {
                    QWidget* child = static_cast<QWidget*>(wit->second.toPointer());
                    if (child) {
                        // Reparent to our central widget
                        child->setParent(centralWidget);
                        layout->addWidget(child);
                    }
                }
            }
        }

        window_->setCentralWidget(centralWidget);
        window_->show();
        window_->raise();
        window_->activateWindow();

        out["window"] = Variant(static_cast<void*>(window_));
    }

private:
    QMainWindow* window_ = nullptr;
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
    ~QtButtonNode() {
        if (button_) {
            button_->deleteLater();
        }
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

        if (!button_) {
            button_ = new QPushButton();
        }
        button_->setText(QString::fromStdString(text));
        button_->setEnabled(enabled);
        if (!style.empty()) {
            button_->setStyleSheet(QString::fromStdString(style));
        }

        out["clicked"] = Variant(false);
        out["widget"] = Variant(static_cast<void*>(button_));
    }

private:
    QPushButton* button_ = nullptr;
};

/// Qt Label node - text display widget
class QtLabelNode final : public Node {
public:
    QtLabelNode() {
        addInput("text", DataType::String);
        addInput("style", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    ~QtLabelNode() {
        if (label_) {
            label_->deleteLater();
        }
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

        if (!label_) {
            label_ = new QLabel();
        }
        label_->setText(QString::fromStdString(text));
        if (!style.empty()) {
            label_->setStyleSheet(QString::fromStdString(style));
        }

        out["widget"] = Variant(static_cast<void*>(label_));
    }

private:
    QLabel* label_ = nullptr;
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
    ~QtLineEditNode() {
        if (lineEdit_) {
            lineEdit_->deleteLater();
        }
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

        if (!lineEdit_) {
            lineEdit_ = new QLineEdit();
        }
        lineEdit_->setPlaceholderText(QString::fromStdString(placeholder));
        lineEdit_->setEnabled(enabled);
        if (!style.empty()) {
            lineEdit_->setStyleSheet(QString::fromStdString(style));
        }

        out["text"] = Variant(lineEdit_->text().toStdString());
        out["widget"] = Variant(static_cast<void*>(lineEdit_));
    }

private:
    QLineEdit* lineEdit_ = nullptr;
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
    ~QtTabWidgetNode() {
        if (tabWidget_) {
            tabWidget_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtTabWidget"; }
    std::string nodeTitle() const override { return "Tab Widget"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string style;
        auto it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        if (!tabWidget_) {
            tabWidget_ = new QTabWidget();
        }
        if (!style.empty()) {
            tabWidget_->setStyleSheet(QString::fromStdString(style));
        }

        // Clear existing tabs and repopulate
        while (tabWidget_->count() > 0) {
            tabWidget_->removeTab(0);
        }
        for (const auto& port : inputs()) {
            const std::string& name = port->name();
            if (name.find("tab") == 0) {
                auto tit = in.find(name);
                if (tit != in.end() && tit->second.isValid() && tit->second.type() == DataType::Pointer) {
                    QWidget* child = static_cast<QWidget*>(tit->second.toPointer());
                    if (child) {
                        tabWidget_->addTab(child, QString::fromStdString(name));
                    }
                }
            }
        }

        out["currentTab"] = Variant(static_cast<int64_t>(tabWidget_->currentIndex()));
        out["widget"] = Variant(static_cast<void*>(tabWidget_));
    }

private:
    QTabWidget* tabWidget_ = nullptr;
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
    ~QtLayoutNode() {
        if (container_) {
            container_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtLayout"; }
    std::string nodeTitle() const override { return "Layout"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string dir = "vertical";
        auto it = in.find("direction");
        if (it != in.end() && it->second.isValid()) dir = it->second.toString().toStdString();

        if (!container_) {
            container_ = new QWidget();
        }

        // Clear old layout
        QLayout* oldLayout = container_->layout();
        if (oldLayout) {
            while (QLayoutItem* item = oldLayout->takeAt(0)) {
                delete item;
            }
            delete oldLayout;
        }

        QBoxLayout* layout = (dir == "horizontal")
            ? static_cast<QBoxLayout*>(new QHBoxLayout(container_))
            : static_cast<QBoxLayout*>(new QVBoxLayout(container_));
        layout->setSpacing(8);
        layout->setContentsMargins(8, 8, 8, 8);

        for (const auto& port : inputs()) {
            const std::string& name = port->name();
            if (name.find("item") == 0) {
                auto iit = in.find(name);
                if (iit != in.end() && iit->second.isValid() && iit->second.type() == DataType::Pointer) {
                    QWidget* child = static_cast<QWidget*>(iit->second.toPointer());
                    if (child) {
                        child->setParent(container_);
                        layout->addWidget(child);
                    }
                }
            }
        }

        out["layout"] = Variant(static_cast<void*>(container_));
    }

private:
    QWidget* container_ = nullptr;
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
    ~QtSliderNode() {
        if (slider_) {
            slider_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtSlider"; }
    std::string nodeTitle() const override { return "Slider"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t minVal = 0, maxVal = 100, val = 50;
        std::string style;

        auto it = in.find("min");
        if (it != in.end() && it->second.isValid()) minVal = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();
        it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        val = std::max(minVal, std::min(maxVal, val));

        if (!slider_) {
            slider_ = new QSlider(Qt::Horizontal);
        }
        slider_->setRange(static_cast<int>(minVal), static_cast<int>(maxVal));
        slider_->setValue(static_cast<int>(val));
        if (!style.empty()) {
            slider_->setStyleSheet(QString::fromStdString(style));
        }

        out["value"] = Variant(val);
        out["widget"] = Variant(static_cast<void*>(slider_));
    }

private:
    QSlider* slider_ = nullptr;
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
    ~QtCheckBoxNode() {
        if (checkBox_) {
            checkBox_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtCheckBox"; }
    std::string nodeTitle() const override { return "Check Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string text = "CheckBox";
        bool checked = false;
        std::string style;

        auto it = in.find("text");
        if (it != in.end() && it->second.isValid()) text = it->second.toString().toStdString();
        it = in.find("checked");
        if (it != in.end() && it->second.isValid()) checked = it->second.toBool();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        if (!checkBox_) {
            checkBox_ = new QCheckBox();
        }
        checkBox_->setText(QString::fromStdString(text));
        checkBox_->setChecked(checked);
        if (!style.empty()) {
            checkBox_->setStyleSheet(QString::fromStdString(style));
        }

        out["checked"] = Variant(checkBox_->isChecked());
        out["widget"] = Variant(static_cast<void*>(checkBox_));
    }

private:
    QCheckBox* checkBox_ = nullptr;
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
    ~QtComboBoxNode() {
        if (comboBox_) {
            comboBox_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtComboBox"; }
    std::string nodeTitle() const override { return "Combo Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        std::string items = "Item1;Item2;Item3";
        std::string style;

        auto it = in.find("items");
        if (it != in.end() && it->second.isValid()) items = it->second.toString().toStdString();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        if (!comboBox_) {
            comboBox_ = new QComboBox();
        }
        comboBox_->clear();
        QStringList itemList = QString::fromStdString(items).split(';');
        for (const QString& item : itemList) {
            if (!item.trimmed().isEmpty()) {
                comboBox_->addItem(item.trimmed());
            }
        }
        if (!style.empty()) {
            comboBox_->setStyleSheet(QString::fromStdString(style));
        }

        out["currentText"] = Variant(comboBox_->currentText().toStdString());
        out["currentIndex"] = Variant(static_cast<int64_t>(comboBox_->currentIndex()));
        out["widget"] = Variant(static_cast<void*>(comboBox_));
    }

private:
    QComboBox* comboBox_ = nullptr;
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
    ~QtSpinBoxNode() {
        if (spinBox_) {
            spinBox_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtSpinBox"; }
    std::string nodeTitle() const override { return "Spin Box"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t minVal = 0, maxVal = 100, val = 0;
        std::string style;

        auto it = in.find("min");
        if (it != in.end() && it->second.isValid()) minVal = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();
        it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        val = std::max(minVal, std::min(maxVal, val));

        if (!spinBox_) {
            spinBox_ = new QSpinBox();
        }
        spinBox_->setRange(static_cast<int>(minVal), static_cast<int>(maxVal));
        spinBox_->setValue(static_cast<int>(val));
        if (!style.empty()) {
            spinBox_->setStyleSheet(QString::fromStdString(style));
        }

        out["value"] = Variant(val);
        out["widget"] = Variant(static_cast<void*>(spinBox_));
    }

private:
    QSpinBox* spinBox_ = nullptr;
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
    ~QtProgressBarNode() {
        if (progressBar_) {
            progressBar_->deleteLater();
        }
    }
    std::string typeName() const override { return "QtProgressBar"; }
    std::string nodeTitle() const override { return "Progress Bar"; }

    void process(const VariantMap& in, VariantMap& out) override {
        int64_t val = 0, maxVal = 100;
        std::string style;

        auto it = in.find("value");
        if (it != in.end() && it->second.isValid()) val = it->second.toInt();
        it = in.find("max");
        if (it != in.end() && it->second.isValid()) maxVal = it->second.toInt();
        it = in.find("style");
        if (it != in.end() && it->second.isValid()) style = it->second.toString().toStdString();

        val = std::max(int64_t(0), std::min(maxVal, val));

        if (!progressBar_) {
            progressBar_ = new QProgressBar();
        }
        progressBar_->setRange(0, static_cast<int>(maxVal));
        progressBar_->setValue(static_cast<int>(val));
        if (!style.empty()) {
            progressBar_->setStyleSheet(QString::fromStdString(style));
        }

        out["widget"] = Variant(static_cast<void*>(progressBar_));
    }

private:
    QProgressBar* progressBar_ = nullptr;
};

// ============================================================================
// Qt Widget Function Nodes (方法调用节点)
// ============================================================================

/// WidgetResize - 调整控件大小
class WidgetResizeNode final : public Node {
public:
    WidgetResizeNode() {
        addInput("widget", DataType::Generic);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetResize"; }
    std::string nodeTitle() const override { return "Resize"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int width = static_cast<int>(in.at("width").toInt());
                int height = static_cast<int>(in.at("height").toInt());
                w->resize(width, height);
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetMove - 移动控件位置
class WidgetMoveNode final : public Node {
public:
    WidgetMoveNode() {
        addInput("widget", DataType::Generic);
        addInput("x", DataType::Integer);
        addInput("y", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetMove"; }
    std::string nodeTitle() const override { return "Move"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int x = static_cast<int>(in.at("x").toInt());
                int y = static_cast<int>(in.at("y").toInt());
                w->move(x, y);
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetShow - 显示控件
class WidgetShowNode final : public Node {
public:
    WidgetShowNode() {
        addInput("widget", DataType::Generic);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetShow"; }
    std::string nodeTitle() const override { return "Show"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->show();
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetHide - 隐藏控件
class WidgetHideNode final : public Node {
public:
    WidgetHideNode() {
        addInput("widget", DataType::Generic);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetHide"; }
    std::string nodeTitle() const override { return "Hide"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->hide();
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetEnabled - 设置启用状态
class WidgetSetEnabledNode final : public Node {
public:
    WidgetSetEnabledNode() {
        addInput("widget", DataType::Generic);
        addInput("enabled", DataType::Boolean);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetEnabled"; }
    std::string nodeTitle() const override { return "Set Enabled"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->setEnabled(in.at("enabled").toBool());
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetVisible - 设置可见性
class WidgetSetVisibleNode final : public Node {
public:
    WidgetSetVisibleNode() {
        addInput("widget", DataType::Generic);
        addInput("visible", DataType::Boolean);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetVisible"; }
    std::string nodeTitle() const override { return "Set Visible"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->setVisible(in.at("visible").toBool());
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetStyleSheet - 设置样式表
class WidgetSetStyleSheetNode final : public Node {
public:
    WidgetSetStyleSheetNode() {
        addInput("widget", DataType::Generic);
        addInput("style", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetStyleSheet"; }
    std::string nodeTitle() const override { return "Set Style"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->setStyleSheet(in.at("style").toString());
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetToolTip - 设置提示文本
class WidgetSetToolTipNode final : public Node {
public:
    WidgetSetToolTipNode() {
        addInput("widget", DataType::Generic);
        addInput("tooltip", DataType::String);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetToolTip"; }
    std::string nodeTitle() const override { return "Set Tooltip"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->setToolTip(in.at("tooltip").toString());
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetFixedSize - 设置固定大小
class WidgetSetFixedSizeNode final : public Node {
public:
    WidgetSetFixedSizeNode() {
        addInput("widget", DataType::Generic);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetFixedSize"; }
    std::string nodeTitle() const override { return "Set Fixed Size"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int width = static_cast<int>(in.at("width").toInt());
                int height = static_cast<int>(in.at("height").toInt());
                w->setFixedSize(width, height);
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetMinimumSize - 设置最小大小
class WidgetSetMinimumSizeNode final : public Node {
public:
    WidgetSetMinimumSizeNode() {
        addInput("widget", DataType::Generic);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetMinimumSize"; }
    std::string nodeTitle() const override { return "Set Min Size"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int width = static_cast<int>(in.at("width").toInt());
                int height = static_cast<int>(in.at("height").toInt());
                w->setMinimumSize(width, height);
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetSetMaximumSize - 设置最大大小
class WidgetSetMaximumSizeNode final : public Node {
public:
    WidgetSetMaximumSizeNode() {
        addInput("widget", DataType::Generic);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetSetMaximumSize"; }
    std::string nodeTitle() const override { return "Set Max Size"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int width = static_cast<int>(in.at("width").toInt());
                int height = static_cast<int>(in.at("height").toInt());
                w->setMaximumSize(width, height);
                out["widget"] = it->second;
            }
        }
    }
};

/// WindowSetTitle - 设置窗口标题
class WindowSetTitleNode final : public Node {
public:
    WindowSetTitleNode() {
        addInput("window", DataType::Generic);
        addInput("title", DataType::String);
        addOutput("window", DataType::Generic);
    }
    std::string typeName() const override { return "WindowSetTitle"; }
    std::string nodeTitle() const override { return "Set Title"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("window");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->setWindowTitle(in.at("title").toString());
                out["window"] = it->second;
            }
        }
    }
};

/// WindowSetGeometry - 设置窗口几何属性
class WindowSetGeometryNode final : public Node {
public:
    WindowSetGeometryNode() {
        addInput("window", DataType::Generic);
        addInput("x", DataType::Integer);
        addInput("y", DataType::Integer);
        addInput("width", DataType::Integer);
        addInput("height", DataType::Integer);
        addOutput("window", DataType::Generic);
    }
    std::string typeName() const override { return "WindowSetGeometry"; }
    std::string nodeTitle() const override { return "Set Geometry"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("window");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                int x = static_cast<int>(in.at("x").toInt());
                int y = static_cast<int>(in.at("y").toInt());
                int width = static_cast<int>(in.at("width").toInt());
                int height = static_cast<int>(in.at("height").toInt());
                w->setGeometry(x, y, width, height);
                out["window"] = it->second;
            }
        }
    }
};

/// LabelSetText - 设置标签文本
class LabelSetTextNode final : public Node {
public:
    LabelSetTextNode() {
        addInput("label", DataType::Generic);
        addInput("text", DataType::String);
        addOutput("label", DataType::Generic);
    }
    std::string typeName() const override { return "LabelSetText"; }
    std::string nodeTitle() const override { return "Set Text"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("label");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QLabel* label = qobject_cast<QLabel*>(static_cast<QWidget*>(it->second.toPointer()));
            if (label) {
                label->setText(in.at("text").toString());
                out["label"] = it->second;
            }
        }
    }
};

/// ButtonSetText - 设置按钮文本
class ButtonSetTextNode final : public Node {
public:
    ButtonSetTextNode() {
        addInput("button", DataType::Generic);
        addInput("text", DataType::String);
        addOutput("button", DataType::Generic);
    }
    std::string typeName() const override { return "ButtonSetText"; }
    std::string nodeTitle() const override { return "Set Text"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("button");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QPushButton* btn = qobject_cast<QPushButton*>(static_cast<QWidget*>(it->second.toPointer()));
            if (btn) {
                btn->setText(in.at("text").toString());
                out["button"] = it->second;
            }
        }
    }
};

/// LineEditSetText - 设置行编辑器文本
class LineEditSetTextNode final : public Node {
public:
    LineEditSetTextNode() {
        addInput("lineEdit", DataType::Generic);
        addInput("text", DataType::String);
        addOutput("lineEdit", DataType::Generic);
    }
    std::string typeName() const override { return "LineEditSetText"; }
    std::string nodeTitle() const override { return "Set Text"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("lineEdit");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QLineEdit* edit = qobject_cast<QLineEdit*>(static_cast<QWidget*>(it->second.toPointer()));
            if (edit) {
                edit->setText(in.at("text").toString());
                out["lineEdit"] = it->second;
            }
        }
    }
};

/// LineEditGetText - 获取行编辑器文本
class LineEditGetTextNode final : public Node {
public:
    LineEditGetTextNode() {
        addInput("lineEdit", DataType::Generic);
        addOutput("text", DataType::String);
        addOutput("lineEdit", DataType::Generic);
    }
    std::string typeName() const override { return "LineEditGetText"; }
    std::string nodeTitle() const override { return "Get Text"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("lineEdit");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QLineEdit* edit = qobject_cast<QLineEdit*>(static_cast<QWidget*>(it->second.toPointer()));
            if (edit) {
                out["text"] = Variant(edit->text().toStdString());
                out["lineEdit"] = it->second;
            }
        }
    }
};

/// SliderSetValue - 设置滑块值
class SliderSetValueNode final : public Node {
public:
    SliderSetValueNode() {
        addInput("slider", DataType::Generic);
        addInput("value", DataType::Integer);
        addOutput("slider", DataType::Generic);
    }
    std::string typeName() const override { return "SliderSetValue"; }
    std::string nodeTitle() const override { return "Set Value"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("slider");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QSlider* slider = qobject_cast<QSlider*>(static_cast<QWidget*>(it->second.toPointer()));
            if (slider) {
                slider->setValue(static_cast<int>(in.at("value").toInt()));
                out["slider"] = it->second;
            }
        }
    }
};

/// SliderGetValue - 获取滑块值
class SliderGetValueNode final : public Node {
public:
    SliderGetValueNode() {
        addInput("slider", DataType::Generic);
        addOutput("value", DataType::Integer);
        addOutput("slider", DataType::Generic);
    }
    std::string typeName() const override { return "SliderGetValue"; }
    std::string nodeTitle() const override { return "Get Value"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("slider");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QSlider* slider = qobject_cast<QSlider*>(static_cast<QWidget*>(it->second.toPointer()));
            if (slider) {
                out["value"] = Variant(static_cast<int64_t>(slider->value()));
                out["slider"] = it->second;
            }
        }
    }
};

/// ProgressBarSetValue - 设置进度条值
class ProgressBarSetValueNode final : public Node {
public:
    ProgressBarSetValueNode() {
        addInput("progressBar", DataType::Generic);
        addInput("value", DataType::Integer);
        addOutput("progressBar", DataType::Generic);
    }
    std::string typeName() const override { return "ProgressBarSetValue"; }
    std::string nodeTitle() const override { return "Set Value"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("progressBar");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QProgressBar* bar = qobject_cast<QProgressBar*>(static_cast<QWidget*>(it->second.toPointer()));
            if (bar) {
                bar->setValue(static_cast<int>(in.at("value").toInt()));
                out["progressBar"] = it->second;
            }
        }
    }
};

/// CheckBoxSetChecked - 设置复选框选中状态
class CheckBoxSetCheckedNode final : public Node {
public:
    CheckBoxSetCheckedNode() {
        addInput("checkBox", DataType::Generic);
        addInput("checked", DataType::Boolean);
        addOutput("checkBox", DataType::Generic);
    }
    std::string typeName() const override { return "CheckBoxSetChecked"; }
    std::string nodeTitle() const override { return "Set Checked"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("checkBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (cb) {
                cb->setChecked(in.at("checked").toBool());
                out["checkBox"] = it->second;
            }
        }
    }
};

/// CheckBoxIsChecked - 获取复选框选中状态
class CheckBoxIsCheckedNode final : public Node {
public:
    CheckBoxIsCheckedNode() {
        addInput("checkBox", DataType::Generic);
        addOutput("checked", DataType::Boolean);
        addOutput("checkBox", DataType::Generic);
    }
    std::string typeName() const override { return "CheckBoxIsChecked"; }
    std::string nodeTitle() const override { return "Is Checked"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("checkBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QCheckBox* cb = qobject_cast<QCheckBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (cb) {
                out["checked"] = Variant(cb->isChecked());
                out["checkBox"] = it->second;
            }
        }
    }
};

/// SpinBoxSetValue - 设置数值框值
class SpinBoxSetValueNode final : public Node {
public:
    SpinBoxSetValueNode() {
        addInput("spinBox", DataType::Generic);
        addInput("value", DataType::Integer);
        addOutput("spinBox", DataType::Generic);
    }
    std::string typeName() const override { return "SpinBoxSetValue"; }
    std::string nodeTitle() const override { return "Set Value"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("spinBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (sb) {
                sb->setValue(static_cast<int>(in.at("value").toInt()));
                out["spinBox"] = it->second;
            }
        }
    }
};

/// SpinBoxGetValue - 获取数值框值
class SpinBoxGetValueNode final : public Node {
public:
    SpinBoxGetValueNode() {
        addInput("spinBox", DataType::Generic);
        addOutput("value", DataType::Integer);
        addOutput("spinBox", DataType::Generic);
    }
    std::string typeName() const override { return "SpinBoxGetValue"; }
    std::string nodeTitle() const override { return "Get Value"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("spinBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (sb) {
                out["value"] = Variant(static_cast<int64_t>(sb->value()));
                out["spinBox"] = it->second;
            }
        }
    }
};

/// ComboBoxAddItem - 添加下拉框项
class ComboBoxAddItemNode final : public Node {
public:
    ComboBoxAddItemNode() {
        addInput("comboBox", DataType::Generic);
        addInput("item", DataType::String);
        addOutput("comboBox", DataType::Generic);
    }
    std::string typeName() const override { return "ComboBoxAddItem"; }
    std::string nodeTitle() const override { return "Add Item"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("comboBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QComboBox* cb = qobject_cast<QComboBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (cb) {
                cb->addItem(in.at("item").toString());
                out["comboBox"] = it->second;
            }
        }
    }
};

/// ComboBoxGetCurrentText - 获取下拉框当前文本
class ComboBoxGetCurrentTextNode final : public Node {
public:
    ComboBoxGetCurrentTextNode() {
        addInput("comboBox", DataType::Generic);
        addOutput("text", DataType::String);
        addOutput("comboBox", DataType::Generic);
    }
    std::string typeName() const override { return "ComboBoxGetCurrentText"; }
    std::string nodeTitle() const override { return "Get Text"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("comboBox");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QComboBox* cb = qobject_cast<QComboBox*>(static_cast<QWidget*>(it->second.toPointer()));
            if (cb) {
                out["text"] = Variant(cb->currentText().toStdString());
                out["comboBox"] = it->second;
            }
        }
    }
};

/// WidgetClose - 关闭窗口
class WidgetCloseNode final : public Node {
public:
    WidgetCloseNode() {
        addInput("widget", DataType::Generic);
        addOutput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetClose"; }
    std::string nodeTitle() const override { return "Close"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->close();
                out["widget"] = it->second;
            }
        }
    }
};

/// WidgetDelete - 删除控件
class WidgetDeleteNode final : public Node {
public:
    WidgetDeleteNode() {
        addInput("widget", DataType::Generic);
    }
    std::string typeName() const override { return "WidgetDelete"; }
    std::string nodeTitle() const override { return "Delete"; }
    void process(const VariantMap& in, VariantMap& out) override {
        auto it = in.find("widget");
        if (it != in.end() && it->second.type() == DataType::Pointer) {
            QWidget* w = static_cast<QWidget*>(it->second.toPointer());
            if (w) {
                w->deleteLater();
            }
        }
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
    // Qt Widget Function nodes
    if (typeName == "WidgetResize")           return std::make_unique<WidgetResizeNode>();
    if (typeName == "WidgetMove")             return std::make_unique<WidgetMoveNode>();
    if (typeName == "WidgetShow")             return std::make_unique<WidgetShowNode>();
    if (typeName == "WidgetHide")             return std::make_unique<WidgetHideNode>();
    if (typeName == "WidgetSetEnabled")       return std::make_unique<WidgetSetEnabledNode>();
    if (typeName == "WidgetSetVisible")       return std::make_unique<WidgetSetVisibleNode>();
    if (typeName == "WidgetSetStyleSheet")    return std::make_unique<WidgetSetStyleSheetNode>();
    if (typeName == "WidgetSetToolTip")       return std::make_unique<WidgetSetToolTipNode>();
    if (typeName == "WidgetSetFixedSize")     return std::make_unique<WidgetSetFixedSizeNode>();
    if (typeName == "WidgetSetMinimumSize")   return std::make_unique<WidgetSetMinimumSizeNode>();
    if (typeName == "WidgetSetMaximumSize")   return std::make_unique<WidgetSetMaximumSizeNode>();
    if (typeName == "WindowSetTitle")         return std::make_unique<WindowSetTitleNode>();
    if (typeName == "WindowSetGeometry")      return std::make_unique<WindowSetGeometryNode>();
    if (typeName == "LabelSetText")           return std::make_unique<LabelSetTextNode>();
    if (typeName == "ButtonSetText")          return std::make_unique<ButtonSetTextNode>();
    if (typeName == "LineEditSetText")        return std::make_unique<LineEditSetTextNode>();
    if (typeName == "LineEditGetText")        return std::make_unique<LineEditGetTextNode>();
    if (typeName == "SliderSetValue")         return std::make_unique<SliderSetValueNode>();
    if (typeName == "SliderGetValue")         return std::make_unique<SliderGetValueNode>();
    if (typeName == "ProgressBarSetValue")    return std::make_unique<ProgressBarSetValueNode>();
    if (typeName == "CheckBoxSetChecked")     return std::make_unique<CheckBoxSetCheckedNode>();
    if (typeName == "CheckBoxIsChecked")      return std::make_unique<CheckBoxIsCheckedNode>();
    if (typeName == "SpinBoxSetValue")        return std::make_unique<SpinBoxSetValueNode>();
    if (typeName == "SpinBoxGetValue")        return std::make_unique<SpinBoxGetValueNode>();
    if (typeName == "ComboBoxAddItem")        return std::make_unique<ComboBoxAddItemNode>();
    if (typeName == "ComboBoxGetCurrentText") return std::make_unique<ComboBoxGetCurrentTextNode>();
    if (typeName == "WidgetClose")            return std::make_unique<WidgetCloseNode>();
    if (typeName == "WidgetDelete")           return std::make_unique<WidgetDeleteNode>();
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