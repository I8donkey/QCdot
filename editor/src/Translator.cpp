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
            {QStringLiteral("app.title"),       QStringLiteral("QBlock - Visual Programming Editor")},

            // ---- Toolbar ----
            {QStringLiteral("toolbar.new"),     QStringLiteral("New")},
            {QStringLiteral("toolbar.open"),    QStringLiteral("Open")},
            {QStringLiteral("toolbar.save"),    QStringLiteral("Save")},
            {QStringLiteral("toolbar.run_dataflow"), QStringLiteral("Run (Dataflow)")},
            {QStringLiteral("toolbar.run_signal"),   QStringLiteral("Run (Signal)")},
            {QStringLiteral("toolbar.fit_view"),     QStringLiteral("Fit View")},
            {QStringLiteral("toolbar.lang"),    QStringLiteral("中文")},

            // ---- Palette ----
            {QStringLiteral("palette.title"),   QStringLiteral("Node Palette")},
            {QStringLiteral("palette.nodes"),   QStringLiteral("Nodes")},

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

            // ---- File dialogs ----
            {QStringLiteral("dialog.open_title"),     QStringLiteral("Open Graph")},
            {QStringLiteral("dialog.save_title"),     QStringLiteral("Save Graph")},
            {QStringLiteral("dialog.open_failed"),    QStringLiteral("Cannot open file: ")},
            {QStringLiteral("dialog.parse_failed"),   QStringLiteral("Failed to parse graph file.")},
            {QStringLiteral("dialog.write_failed"),   QStringLiteral("Cannot write file: ")},
            {QStringLiteral("dialog.filter"),         QStringLiteral("QBlock (*.qblock)")},
        }},

        {QStringLiteral("zh"), {
            // ---- Application ----
            {QStringLiteral("app.title"),       QStringLiteral("QBlock - 可视化编程编辑器")},

            // ---- Toolbar ----
            {QStringLiteral("toolbar.new"),     QStringLiteral("新建")},
            {QStringLiteral("toolbar.open"),    QStringLiteral("打开")},
            {QStringLiteral("toolbar.save"),    QStringLiteral("保存")},
            {QStringLiteral("toolbar.run_dataflow"), QStringLiteral("运行（数据流）")},
            {QStringLiteral("toolbar.run_signal"),   QStringLiteral("运行（信号）")},
            {QStringLiteral("toolbar.fit_view"),     QStringLiteral("适配视图")},
            {QStringLiteral("toolbar.lang"),    QStringLiteral("English")},

            // ---- Palette ----
            {QStringLiteral("palette.title"),   QStringLiteral("节点素材库")},
            {QStringLiteral("palette.nodes"),   QStringLiteral("节点")},

            // ---- Node names (palette display) ----
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

            // ---- File dialogs ----
            {QStringLiteral("dialog.open_title"),     QStringLiteral("打开流程图")},
            {QStringLiteral("dialog.save_title"),     QStringLiteral("保存流程图")},
            {QStringLiteral("dialog.open_failed"),    QStringLiteral("无法打开文件：")},
            {QStringLiteral("dialog.parse_failed"),   QStringLiteral("解析流程图文件失败。")},
            {QStringLiteral("dialog.write_failed"),   QStringLiteral("无法写入文件：")},
            {QStringLiteral("dialog.filter"),         QStringLiteral("QBlock 流程图 (*.qblock)")},
        }}
    };
    return s_data;
}

} // namespace QBlock