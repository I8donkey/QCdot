#include <QBlock/Translator.h>

namespace QBlock {

QString Translator::s_currentLang = QStringLiteral("en");
std::vector<Translator::Callback> Translator::s_callbacks;

void Translator::setLanguage(const QString& lang) {
    if (lang == s_currentLang) return;
    if (lang != QStringLiteral("en") && lang != QStringLiteral("zh")) return;
    s_currentLang = lang;
    for (auto& cb : s_callbacks) {
        if (cb) cb();
    }
}

QString Translator::currentLanguage() {
    return s_currentLang;
}

QString Translator::tr(const QString& key) {
    const auto& all = data();
    auto it = all.find(s_currentLang);
    if (it == all.end()) return key;
    auto jt = it->find(key);
    if (jt == it->end()) return key;
    return jt.value();
}

void Translator::onLanguageChanged(Callback cb) {
    s_callbacks.push_back(std::move(cb));
}

void Translator::clearCallbacks() {
    s_callbacks.clear();
}

const QHash<QString, QHash<QString, QString>>& Translator::data() {
    static const QHash<QString, QHash<QString, QString>> s_data = {
        {QStringLiteral("en"), {
            // ---- Application ----
            {QStringLiteral("app.title"),       QStringLiteral("qcdot - Visual Programming Editor")},

            // ---- Toolbar ----
            {QStringLiteral("toolbar.new"),     QStringLiteral("New")},
            {QStringLiteral("toolbar.open"),    QStringLiteral("Open")},
            {QStringLiteral("toolbar.save"),    QStringLiteral("Save")},
            {QStringLiteral("toolbar.run_dataflow"), QStringLiteral("Run (Dataflow)")},
            {QStringLiteral("toolbar.run_signal"),   QStringLiteral("Run (Signal)")},
            {QStringLiteral("toolbar.fit_view"),     QStringLiteral("Fit View")},
            {QStringLiteral("toolbar.copy"),    QStringLiteral("Copy")},
            {QStringLiteral("toolbar.paste"),    QStringLiteral("Paste")},
            {QStringLiteral("toolbar.lang"),    QStringLiteral("Language")},
            {QStringLiteral("toolbar.settings"), QStringLiteral("Settings")},
            {QStringLiteral("toolbar.theme_dark"),   QStringLiteral("Dark Theme")},
            {QStringLiteral("toolbar.theme_light"),  QStringLiteral("Light Theme")},
            {QStringLiteral("toolbar.export_cpp"),   QStringLiteral("Export as C++")},

            // ---- Dialogs ----
            {QStringLiteral("dialog.open_title"), QStringLiteral("Open Graph")},
            {QStringLiteral("dialog.save_title"), QStringLiteral("Save Graph")},
            {QStringLiteral("dialog.export_title"),QStringLiteral("Export as C++ Source")},
            {QStringLiteral("dialog.filter"),         QStringLiteral("QCdot (*.qcd)")},
            {QStringLiteral("dialog.export_empty"),   QStringLiteral("Graph is empty. Nothing to export.")},

            // ---- Node Picker ----
            {QStringLiteral("picker.title"),  QStringLiteral("Select Node")},
            {QStringLiteral("picker.prompt"), QStringLiteral("Select a node to create and connect:")},
            {QStringLiteral("picker.cancel"), QStringLiteral("Cancel")},
            {QStringLiteral("picker.search"), QStringLiteral("Search...")},
            {QStringLiteral("palette.title"),   QStringLiteral("Node Palette")},
            {QStringLiteral("palette.nodes"),   QStringLiteral("Nodes")},

            // ---- Common port names ----
            {QStringLiteral("port.value"),      QStringLiteral("value")},
            {QStringLiteral("port.result"),     QStringLiteral("result")},
            {QStringLiteral("port.out"),        QStringLiteral("out")},
            {QStringLiteral("port.in"),         QStringLiteral("in")},
            {QStringLiteral("port.input"),      QStringLiteral("input")},
            {QStringLiteral("port.output"),     QStringLiteral("output")},
            {QStringLiteral("port.a"),          QStringLiteral("a")},
            {QStringLiteral("port.b"),          QStringLiteral("b")},
            {QStringLiteral("port.x"),          QStringLiteral("x")},
            {QStringLiteral("port.y"),          QStringLiteral("y")},
            {QStringLiteral("port.base"),       QStringLiteral("base")},
            {QStringLiteral("port.exp"),        QStringLiteral("exp")},
            {QStringLiteral("port.str"),        QStringLiteral("str")},
            {QStringLiteral("port.start"),      QStringLiteral("start")},
            {QStringLiteral("port.length"),     QStringLiteral("length")},
            {QStringLiteral("port.bits"),       QStringLiteral("bits")},
            {QStringLiteral("port.path"),       QStringLiteral("path")},
            {QStringLiteral("port.content"),    QStringLiteral("content")},
            {QStringLiteral("port.success"),    QStringLiteral("success")},
            {QStringLiteral("port.data"),       QStringLiteral("data")},
            {QStringLiteral("port.min"),        QStringLiteral("min")},
            {QStringLiteral("port.max"),        QStringLiteral("max")},
            {QStringLiteral("port.ms"),         QStringLiteral("ms")},
            {QStringLiteral("port.done"),       QStringLiteral("done")},
            {QStringLiteral("port.condition"),  QStringLiteral("condition")},
            {QStringLiteral("port.trueValue"),  QStringLiteral("trueValue")},
            {QStringLiteral("port.falseValue"), QStringLiteral("falseValue")},
            {QStringLiteral("port.true"),       QStringLiteral("true")},
            {QStringLiteral("port.false"),      QStringLiteral("false")},
            {QStringLiteral("port.then"),       QStringLiteral("then")},
            {QStringLiteral("port.else"),       QStringLiteral("else")},
            {QStringLiteral("port.exec"),       QStringLiteral("exec")},

            // ---- Qt UI port names ----
            {QStringLiteral("port.title"),       QStringLiteral("title")},
            {QStringLiteral("port.width"),       QStringLiteral("width")},
            {QStringLiteral("port.height"),      QStringLiteral("height")},
            {QStringLiteral("port.window"),      QStringLiteral("window")},
            {QStringLiteral("port.text"),        QStringLiteral("text")},
            {QStringLiteral("port.enabled"),     QStringLiteral("enabled")},
            {QStringLiteral("port.widget"),      QStringLiteral("widget")},
            {QStringLiteral("port.tab"),         QStringLiteral("tab")},
            {QStringLiteral("port.item"),        QStringLiteral("item")},
            {QStringLiteral("port.clicked"),     QStringLiteral("clicked")},
            {QStringLiteral("port.placeholder"), QStringLiteral("placeholder")},
            {QStringLiteral("port.currentText"), QStringLiteral("currentText")},
            {QStringLiteral("port.currentIndex"), QStringLiteral("currentIndex")},
            {QStringLiteral("port.items"),       QStringLiteral("items")},
            {QStringLiteral("port.tab1"),        QStringLiteral("tab1")},
            {QStringLiteral("port.tab2"),        QStringLiteral("tab2")},
            {QStringLiteral("port.tab3"),        QStringLiteral("tab3")},
            {QStringLiteral("port.item1"),       QStringLiteral("item1")},
            {QStringLiteral("port.item2"),       QStringLiteral("item2")},
            {QStringLiteral("port.item3"),       QStringLiteral("item3")},
            {QStringLiteral("port.direction"),   QStringLiteral("direction")},
            {QStringLiteral("port.layout"),      QStringLiteral("layout")},
            {QStringLiteral("port.checked"),     QStringLiteral("checked")},
            {QStringLiteral("port.color"),       QStringLiteral("color")},
            {QStringLiteral("port.widget1"),     QStringLiteral("widget1")},
            {QStringLiteral("port.widget2"),     QStringLiteral("widget2")},
            {QStringLiteral("port.widget3"),     QStringLiteral("widget3")},
            {QStringLiteral("port.r"),           QStringLiteral("r")},
            {QStringLiteral("port.g"),           QStringLiteral("g")},
            {QStringLiteral("port.b"),           QStringLiteral("b")},
            {QStringLiteral("port.a"),           QStringLiteral("a")},

            // ---- Input/output-specific port labels ----
            {QStringLiteral("port.input.value"),   QStringLiteral("input")},
            {QStringLiteral("port.output.value"),  QStringLiteral("output")},
            {QStringLiteral("port.input.result"),  QStringLiteral("input")},
            {QStringLiteral("port.output.result"), QStringLiteral("result")},
            {QStringLiteral("port.input.a"),      QStringLiteral("a")},
            {QStringLiteral("port.input.b"),      QStringLiteral("b")},
            {QStringLiteral("port.input.x"),      QStringLiteral("x")},
            {QStringLiteral("port.input.y"),      QStringLiteral("y")},
            {QStringLiteral("port.input.str"),    QStringLiteral("str")},
            {QStringLiteral("port.output.str"),   QStringLiteral("str")},

            // ---- Node names (palette display) ----
            {QStringLiteral("node.Add"),              QStringLiteral("Add")},
            {QStringLiteral("node.Subtract"),         QStringLiteral("Subtract")},
            {QStringLiteral("node.Multiply"),         QStringLiteral("Multiply")},
            {QStringLiteral("node.Divide"),           QStringLiteral("Divide")},
            {QStringLiteral("node.Modulo"),           QStringLiteral("Modulo")},
            {QStringLiteral("node.Power"),            QStringLiteral("Power")},
            {QStringLiteral("node.Sqrt"),             QStringLiteral("Sqrt")},
            {QStringLiteral("node.Equal"),            QStringLiteral("Equal")},
            {QStringLiteral("node.GreaterThan"),      QStringLiteral("GreaterThan")},
            {QStringLiteral("node.LessThan"),         QStringLiteral("LessThan")},
            {QStringLiteral("node.And"),              QStringLiteral("And")},
            {QStringLiteral("node.Or"),               QStringLiteral("Or")},
            {QStringLiteral("node.Not"),              QStringLiteral("Not")},
            {QStringLiteral("node.IfElse"),           QStringLiteral("IfElse")},
            {QStringLiteral("node.IfElseBranch"),     QStringLiteral("IfElseBranch")},
            {QStringLiteral("node.ConstantInt"),      QStringLiteral("ConstantInt")},
            {QStringLiteral("node.ConstantFloat"),    QStringLiteral("ConstantFloat")},
            {QStringLiteral("node.ConstantBool"),     QStringLiteral("ConstantBool")},
            {QStringLiteral("node.ConstantString"),   QStringLiteral("ConstantString")},
            {QStringLiteral("node.IntToFloat"),       QStringLiteral("IntToFloat")},
            {QStringLiteral("node.FloatToInt"),       QStringLiteral("FloatToInt")},
            {QStringLiteral("node.ToString"),         QStringLiteral("ToString")},
            {QStringLiteral("node.StringConcat"),     QStringLiteral("StringConcat")},
            {QStringLiteral("node.StringLength"),     QStringLiteral("StringLength")},
            {QStringLiteral("node.StringSubstring"),  QStringLiteral("StringSubstring")},
            {QStringLiteral("node.Print"),            QStringLiteral("Print")},
            {QStringLiteral("node.BinaryAnd"),        QStringLiteral("BinaryAnd")},
            {QStringLiteral("node.BinaryOr"),         QStringLiteral("BinaryOr")},
            {QStringLiteral("node.BinaryXor"),        QStringLiteral("BinaryXor")},
            {QStringLiteral("node.ShiftLeft"),        QStringLiteral("ShiftLeft")},
            {QStringLiteral("node.ShiftRight"),       QStringLiteral("ShiftRight")},
            {QStringLiteral("node.FileReadText"),     QStringLiteral("FileReadText")},
            {QStringLiteral("node.FileWriteText"),    QStringLiteral("FileWriteText")},
            {QStringLiteral("node.FileReadBinary"),   QStringLiteral("FileReadBinary")},
            {QStringLiteral("node.RandomInt"),        QStringLiteral("RandomInt")},
            {QStringLiteral("node.Sleep"),            QStringLiteral("Sleep")},
            {QStringLiteral("node.Variable"),         QStringLiteral("Variable")},
            {QStringLiteral("node.Counter"),          QStringLiteral("Counter")},
            // Qt UI nodes
            {QStringLiteral("node.Color"),            QStringLiteral("Color")},
            {QStringLiteral("node.QtMainWindow"),     QStringLiteral("Main Window")},
            {QStringLiteral("node.QtButton"),         QStringLiteral("Button")},
            {QStringLiteral("node.QtLabel"),          QStringLiteral("Label")},
            {QStringLiteral("node.QtLineEdit"),       QStringLiteral("Line Edit")},
            {QStringLiteral("node.QtTabWidget"),      QStringLiteral("Tab Widget")},
            {QStringLiteral("node.QtLayout"),         QStringLiteral("Layout")},
            {QStringLiteral("node.QtSlider"),         QStringLiteral("Slider")},
            {QStringLiteral("node.QtCheckBox"),       QStringLiteral("Check Box")},
            {QStringLiteral("node.QtComboBox"),       QStringLiteral("Combo Box")},
            {QStringLiteral("node.QtSpinBox"),        QStringLiteral("Spin Box")},
            {QStringLiteral("node.QtProgressBar"),    QStringLiteral("Progress Bar")},
            // Additional node types used by picker
            {QStringLiteral("node.Negate"),           QStringLiteral("Negate")},
            {QStringLiteral("node.Abs"),              QStringLiteral("Abs")},
            {QStringLiteral("node.Min"),              QStringLiteral("Min")},
            {QStringLiteral("node.Max"),              QStringLiteral("Max")},
            {QStringLiteral("node.Clamp"),            QStringLiteral("Clamp")},
            {QStringLiteral("node.IfThen"),           QStringLiteral("IfThen")},
            {QStringLiteral("node.Compare"),          QStringLiteral("Compare")},
            {QStringLiteral("node.VariableGet"),      QStringLiteral("VariableGet")},
            {QStringLiteral("node.VariableSet"),      QStringLiteral("VariableSet")},
            {QStringLiteral("node.Concat"),           QStringLiteral("Concat")},
            {QStringLiteral("node.Substring"),        QStringLiteral("Substring")},
            {QStringLiteral("node.Length"),           QStringLiteral("Length")},
            {QStringLiteral("node.IntToString"),      QStringLiteral("IntToString")},
            {QStringLiteral("node.FloatToString"),    QStringLiteral("FloatToString")},
            {QStringLiteral("node.StringToInt"),      QStringLiteral("StringToInt")},
            {QStringLiteral("node.StringToFloat"),    QStringLiteral("StringToFloat")},
            {QStringLiteral("node.InputInt"),         QStringLiteral("InputInt")},
            {QStringLiteral("node.InputFloat"),       QStringLiteral("InputFloat")},
            {QStringLiteral("node.InputString"),      QStringLiteral("InputString")},
        }},

        {QStringLiteral("zh"), {
            // ---- Application ----
            {QStringLiteral("app.title"),       QStringLiteral("QCdot 可视化编程编辑器")},

            // ---- Toolbar ----
            {QStringLiteral("toolbar.new"),     QStringLiteral("新建")},
            {QStringLiteral("toolbar.open"),    QStringLiteral("打开")},
            {QStringLiteral("toolbar.save"),    QStringLiteral("保存")},
            {QStringLiteral("toolbar.run_dataflow"), QStringLiteral("运行（数据流）")},
            {QStringLiteral("toolbar.run_signal"),   QStringLiteral("运行（信号）")},
            {QStringLiteral("toolbar.fit_view"),     QStringLiteral("适配视图")},
            {QStringLiteral("toolbar.copy"),    QStringLiteral("复制")},
            {QStringLiteral("toolbar.paste"),    QStringLiteral("粘贴")},
            {QStringLiteral("toolbar.lang"),    QStringLiteral("切换语言")},
            {QStringLiteral("toolbar.settings"), QStringLiteral("设置")},
            {QStringLiteral("toolbar.theme_dark"),   QStringLiteral("暗色主题")},
            {QStringLiteral("toolbar.theme_light"),  QStringLiteral("亮色主题")},
            {QStringLiteral("toolbar.export_cpp"),   QStringLiteral("导出为 C++")},

            // ---- Palette ----
            {QStringLiteral("palette.title"),   QStringLiteral("节点素材库")},
            {QStringLiteral("palette.nodes"),   QStringLiteral("节点")},

            // ---- Common port names ----
            {QStringLiteral("port.value"),      QStringLiteral("输入值")},
            {QStringLiteral("port.result"),     QStringLiteral("结果")},
            {QStringLiteral("port.out"),        QStringLiteral("输出")},
            {QStringLiteral("port.in"),         QStringLiteral("输入")},
            {QStringLiteral("port.input"),      QStringLiteral("输入")},
            {QStringLiteral("port.output"),     QStringLiteral("输出")},
            {QStringLiteral("port.a"),          QStringLiteral("a")},
            {QStringLiteral("port.b"),          QStringLiteral("b")},
            {QStringLiteral("port.x"),          QStringLiteral("x")},
            {QStringLiteral("port.y"),          QStringLiteral("y")},
            {QStringLiteral("port.base"),       QStringLiteral("底数")},
            {QStringLiteral("port.exp"),        QStringLiteral("指数")},
            {QStringLiteral("port.str"),        QStringLiteral("字符串")},
            {QStringLiteral("port.start"),      QStringLiteral("起始")},
            {QStringLiteral("port.length"),     QStringLiteral("长度")},
            {QStringLiteral("port.bits"),       QStringLiteral("位数")},
            {QStringLiteral("port.path"),       QStringLiteral("路径")},
            {QStringLiteral("port.content"),    QStringLiteral("内容")},
            {QStringLiteral("port.success"),    QStringLiteral("成功")},
            {QStringLiteral("port.data"),       QStringLiteral("数据")},
            {QStringLiteral("port.min"),        QStringLiteral("最小值")},
            {QStringLiteral("port.max"),        QStringLiteral("最大值")},
            {QStringLiteral("port.ms"),         QStringLiteral("毫秒")},
            {QStringLiteral("port.done"),       QStringLiteral("完成")},
            {QStringLiteral("port.condition"),  QStringLiteral("条件")},
            {QStringLiteral("port.trueValue"),  QStringLiteral("真值")},
            {QStringLiteral("port.falseValue"), QStringLiteral("假值")},
            {QStringLiteral("port.true"),       QStringLiteral("真")},
            {QStringLiteral("port.false"),      QStringLiteral("假")},
            {QStringLiteral("port.then"),       QStringLiteral("真分支")},
            {QStringLiteral("port.else"),       QStringLiteral("假分支")},
            {QStringLiteral("port.exec"),       QStringLiteral("执行")},

            // ---- Qt UI port names ----
            {QStringLiteral("port.title"),       QStringLiteral("标题")},
            {QStringLiteral("port.width"),       QStringLiteral("宽度")},
            {QStringLiteral("port.height"),      QStringLiteral("高度")},
            {QStringLiteral("port.window"),      QStringLiteral("窗口")},
            {QStringLiteral("port.text"),        QStringLiteral("文本")},
            {QStringLiteral("port.enabled"),     QStringLiteral("启用")},
            {QStringLiteral("port.widget"),      QStringLiteral("控件")},
            {QStringLiteral("port.tab"),         QStringLiteral("标签")},
            {QStringLiteral("port.item"),        QStringLiteral("项目")},
            {QStringLiteral("port.clicked"),     QStringLiteral("点击")},
            {QStringLiteral("port.placeholder"), QStringLiteral("占位符")},
            {QStringLiteral("port.currentText"), QStringLiteral("当前文本")},
            {QStringLiteral("port.currentIndex"), QStringLiteral("当前索引")},
            {QStringLiteral("port.items"),       QStringLiteral("选项")},
            {QStringLiteral("port.tab1"),        QStringLiteral("标签1")},
            {QStringLiteral("port.tab2"),        QStringLiteral("标签2")},
            {QStringLiteral("port.tab3"),        QStringLiteral("标签3")},
            {QStringLiteral("port.item1"),       QStringLiteral("项目1")},
            {QStringLiteral("port.item2"),       QStringLiteral("项目2")},
            {QStringLiteral("port.item3"),       QStringLiteral("项目3")},
            {QStringLiteral("port.direction"),   QStringLiteral("方向")},
            {QStringLiteral("port.layout"),      QStringLiteral("布局")},
            {QStringLiteral("port.checked"),     QStringLiteral("选中")},
            {QStringLiteral("port.color"),       QStringLiteral("颜色")},
            {QStringLiteral("port.widget1"),     QStringLiteral("控件1")},

            // ---- Input/output-specific port labels ----
            {QStringLiteral("port.input.value"),   QStringLiteral("输入值")},
            {QStringLiteral("port.output.value"),  QStringLiteral("输出值")},
            {QStringLiteral("port.input.result"),  QStringLiteral("输入")},
            {QStringLiteral("port.output.result"), QStringLiteral("结果")},
            {QStringLiteral("port.input.a"),      QStringLiteral("a")},
            {QStringLiteral("port.input.b"),      QStringLiteral("b")},
            {QStringLiteral("port.input.x"),      QStringLiteral("x")},
            {QStringLiteral("port.input.y"),      QStringLiteral("y")},
            {QStringLiteral("port.input.str"),    QStringLiteral("字符串")},
            {QStringLiteral("port.output.str"),   QStringLiteral("字符串")},

            // ---- Node names ----
            {QStringLiteral("node.Add"),              QStringLiteral("加法")},
            {QStringLiteral("node.Subtract"),         QStringLiteral("减法")},
            {QStringLiteral("node.Multiply"),         QStringLiteral("乘法")},
            {QStringLiteral("node.Divide"),           QStringLiteral("除法")},
            {QStringLiteral("node.Modulo"),           QStringLiteral("取模")},
            {QStringLiteral("node.Power"),            QStringLiteral("幂运算")},
            {QStringLiteral("node.Sqrt"),             QStringLiteral("平方根")},
            {QStringLiteral("node.Equal"),            QStringLiteral("等于")},
            {QStringLiteral("node.GreaterThan"),      QStringLiteral("大于")},
            {QStringLiteral("node.LessThan"),         QStringLiteral("小于")},
            {QStringLiteral("node.And"),              QStringLiteral("与")},
            {QStringLiteral("node.Or"),               QStringLiteral("或")},
            {QStringLiteral("node.Not"),              QStringLiteral("非")},
            {QStringLiteral("node.IfElse"),           QStringLiteral("条件判断")},
            {QStringLiteral("node.IfElseBranch"),     QStringLiteral("条件分支")},
            {QStringLiteral("node.ConstantInt"),      QStringLiteral("整数常量")},
            {QStringLiteral("node.ConstantFloat"),    QStringLiteral("浮点常量")},
            {QStringLiteral("node.ConstantBool"),     QStringLiteral("布尔常量")},
            {QStringLiteral("node.ConstantString"),   QStringLiteral("字符串常量")},
            {QStringLiteral("node.IntToFloat"),       QStringLiteral("整数转浮点")},
            {QStringLiteral("node.FloatToInt"),       QStringLiteral("浮点转整数")},
            {QStringLiteral("node.ToString"),         QStringLiteral("转字符串")},
            {QStringLiteral("node.StringConcat"),     QStringLiteral("字符串拼接")},
            {QStringLiteral("node.StringLength"),     QStringLiteral("字符串长度")},
            {QStringLiteral("node.StringSubstring"),  QStringLiteral("取子串")},
            {QStringLiteral("node.Print"),            QStringLiteral("打印输出")},
            {QStringLiteral("node.BinaryAnd"),        QStringLiteral("按位与")},
            {QStringLiteral("node.BinaryOr"),         QStringLiteral("按位或")},
            {QStringLiteral("node.BinaryXor"),        QStringLiteral("按位异或")},
            {QStringLiteral("node.ShiftLeft"),        QStringLiteral("左移")},
            {QStringLiteral("node.ShiftRight"),       QStringLiteral("右移")},
            {QStringLiteral("node.FileReadText"),     QStringLiteral("读取文本文件")},
            {QStringLiteral("node.FileWriteText"),    QStringLiteral("写入文本文件")},
            {QStringLiteral("node.FileReadBinary"),   QStringLiteral("读取二进制文件")},
            {QStringLiteral("node.RandomInt"),        QStringLiteral("随机整数")},
            {QStringLiteral("node.Sleep"),            QStringLiteral("延时等待")},
            {QStringLiteral("node.Variable"),         QStringLiteral("变量")},
            {QStringLiteral("node.Counter"),          QStringLiteral("计数器")},
            // Qt UI nodes
            {QStringLiteral("node.Color"),            QStringLiteral("颜色")},
            {QStringLiteral("node.QtMainWindow"),     QStringLiteral("主窗口")},
            {QStringLiteral("node.QtButton"),         QStringLiteral("按钮")},
            {QStringLiteral("node.QtLabel"),          QStringLiteral("标签")},
            {QStringLiteral("node.QtLineEdit"),       QStringLiteral("单行输入")},
            {QStringLiteral("node.QtTabWidget"),      QStringLiteral("标签页")},
            {QStringLiteral("node.QtLayout"),         QStringLiteral("布局")},
            {QStringLiteral("node.QtSlider"),         QStringLiteral("滑块")},
            {QStringLiteral("node.QtCheckBox"),       QStringLiteral("复选框")},
            {QStringLiteral("node.QtComboBox"),       QStringLiteral("下拉框")},
            {QStringLiteral("node.QtSpinBox"),        QStringLiteral("数值输入")},
            {QStringLiteral("node.QtProgressBar"),    QStringLiteral("进度条")},
            // Additional node types used by picker
            {QStringLiteral("node.Negate"),           QStringLiteral("取反")},
            {QStringLiteral("node.Abs"),              QStringLiteral("绝对值")},
            {QStringLiteral("node.Min"),              QStringLiteral("最小值")},
            {QStringLiteral("node.Max"),              QStringLiteral("最大值")},
            {QStringLiteral("node.Clamp"),            QStringLiteral("范围限制")},
            {QStringLiteral("node.IfThen"),           QStringLiteral("条件")},
            {QStringLiteral("node.Compare"),          QStringLiteral("比较")},
            {QStringLiteral("node.VariableGet"),      QStringLiteral("读取变量")},
            {QStringLiteral("node.VariableSet"),      QStringLiteral("写入变量")},
            {QStringLiteral("node.Concat"),           QStringLiteral("拼接")},
            {QStringLiteral("node.Substring"),        QStringLiteral("子串")},
            {QStringLiteral("node.Length"),           QStringLiteral("长度")},
            {QStringLiteral("node.IntToString"),      QStringLiteral("整数转字符串")},
            {QStringLiteral("node.FloatToString"),    QStringLiteral("浮点转字符串")},
            {QStringLiteral("node.StringToInt"),      QStringLiteral("字符串转整数")},
            {QStringLiteral("node.StringToFloat"),    QStringLiteral("字符串转浮点")},
            {QStringLiteral("node.InputInt"),         QStringLiteral("输入整数")},
            {QStringLiteral("node.InputFloat"),       QStringLiteral("输入浮点数")},
            {QStringLiteral("node.InputString"),      QStringLiteral("输入字符串")},

            // ---- Node Picker ----
            {QStringLiteral("picker.title"),  QStringLiteral("选择节点")},
            {QStringLiteral("picker.prompt"), QStringLiteral("选择一个节点来创建并连接：")},
            {QStringLiteral("picker.cancel"), QStringLiteral("取消")},
            {QStringLiteral("picker.search"), QStringLiteral("搜索...")},

            // ---- File dialogs ----
            {QStringLiteral("dialog.open_title"),     QStringLiteral("打开流程图")},
            {QStringLiteral("dialog.save_title"),     QStringLiteral("保存流程图")},
            {QStringLiteral("dialog.export_title"),   QStringLiteral("导出为 C++ 源码")},
            {QStringLiteral("dialog.export_empty"),   QStringLiteral("流程图为空，无可导出的内容。")},
            {QStringLiteral("dialog.open_failed"),    QStringLiteral("无法打开文件：")},
            {QStringLiteral("dialog.parse_failed"),   QStringLiteral("解析流程图文件失败。")},
            {QStringLiteral("dialog.write_failed"),   QStringLiteral("无法写入文件：")},
            {QStringLiteral("dialog.filter"),         QStringLiteral("QCdot 流程图 (*.qcd)")},
        }}
    };
    return s_data;
}

} // namespace QBlock