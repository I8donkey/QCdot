#include <QBlock/ThemeManager.h>
#include <vector>

namespace QBlock {

ThemeManager& ThemeManager::instance() {
    static ThemeManager mgr;
    return mgr;
}

static std::vector<ThemeManager::ThemeCallback> s_callbacks;

void ThemeManager::setTheme(ThemeMode mode) {
    instance().theme_ = mode;
    for (auto& cb : s_callbacks) {
        if (cb) cb();
    }
}

void ThemeManager::onThemeChanged(ThemeCallback cb) {
    s_callbacks.push_back(std::move(cb));
}

// ---- Dark theme colors ----
namespace {
    constexpr auto kDarkBg              = QColor(28, 28, 34);
    constexpr auto kDarkGridMinor       = QColor(40, 40, 50);
    constexpr auto kDarkGridMajor       = QColor(50, 50, 60);
    constexpr auto kDarkPanelBg         = QColor(37, 37, 45);
    constexpr auto kDarkPanelBorder     = QColor(60, 60, 70);
    constexpr auto kDarkTextPrimary     = QColor(220, 220, 220);
    constexpr auto kDarkTextSecondary   = QColor(150, 150, 150);
    constexpr auto kDarkNodeBg          = QColor(40, 40, 50);
    constexpr auto kDarkNodeBgSelected  = QColor(55, 55, 70);
    constexpr auto kDarkNodeBorder      = QColor(70, 70, 80);
    constexpr auto kDarkNodeBorderSel   = QColor(100, 140, 255);
    constexpr auto kDarkNodeHdrFrom     = QColor(60, 60, 80);
    constexpr auto kDarkNodeHdrTo       = QColor(45, 45, 60);
    constexpr auto kDarkToolbarBg       = QColor(51, 51, 51);
    constexpr auto kDarkToolbarBtn      = QColor(68, 68, 68);
    constexpr auto kDarkToolbarBtnHover = QColor(85, 85, 85);

    constexpr auto kLightBg             = QColor(240, 240, 245);
    constexpr auto kLightGridMinor      = QColor(220, 220, 230);
    constexpr auto kLightGridMajor      = QColor(200, 200, 210);
    constexpr auto kLightPanelBg        = QColor(250, 250, 252);
    constexpr auto kLightPanelBorder    = QColor(200, 200, 210);
    constexpr auto kLightTextPrimary    = QColor(40, 40, 50);
    constexpr auto kLightTextSecondary  = QColor(120, 120, 130);
    constexpr auto kLightNodeBg         = QColor(245, 245, 250);
    constexpr auto kLightNodeBgSelected = QColor(230, 235, 255);
    constexpr auto kLightNodeBorder     = QColor(200, 200, 210);
    constexpr auto kLightNodeBorderSel  = QColor(80, 130, 255);
    constexpr auto kLightNodeHdrFrom    = QColor(220, 225, 240);
    constexpr auto kLightNodeHdrTo      = QColor(235, 238, 248);
    constexpr auto kLightToolbarBg      = QColor(230, 230, 235);
    constexpr auto kLightToolbarBtn     = QColor(215, 215, 225);
    constexpr auto kLightToolbarBtnHover= QColor(200, 200, 215);
}

QColor ThemeManager::background() {
    return instance().theme_ == ThemeMode::Dark ? kDarkBg : kLightBg;
}
QColor ThemeManager::gridMinor() {
    return instance().theme_ == ThemeMode::Dark ? kDarkGridMinor : kLightGridMinor;
}
QColor ThemeManager::gridMajor() {
    return instance().theme_ == ThemeMode::Dark ? kDarkGridMajor : kLightGridMajor;
}
QColor ThemeManager::panelBg() {
    return instance().theme_ == ThemeMode::Dark ? kDarkPanelBg : kLightPanelBg;
}
QColor ThemeManager::panelBorder() {
    return instance().theme_ == ThemeMode::Dark ? kDarkPanelBorder : kLightPanelBorder;
}
QColor ThemeManager::textPrimary() {
    return instance().theme_ == ThemeMode::Dark ? kDarkTextPrimary : kLightTextPrimary;
}
QColor ThemeManager::textSecondary() {
    return instance().theme_ == ThemeMode::Dark ? kDarkTextSecondary : kLightTextSecondary;
}
QColor ThemeManager::nodeBg() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeBg : kLightNodeBg;
}
QColor ThemeManager::nodeBgSelected() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeBgSelected : kLightNodeBgSelected;
}
QColor ThemeManager::nodeBorder() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeBorder : kLightNodeBorder;
}
QColor ThemeManager::nodeBorderSelected() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeBorderSel : kLightNodeBorderSel;
}
QColor ThemeManager::nodeHeaderFrom() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeHdrFrom : kLightNodeHdrFrom;
}
QColor ThemeManager::nodeHeaderTo() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeHdrTo : kLightNodeHdrTo;
}
QColor ThemeManager::toolbarBg() {
    return instance().theme_ == ThemeMode::Dark ? kDarkToolbarBg : kLightToolbarBg;
}
QColor ThemeManager::toolbarBtn() {
    return instance().theme_ == ThemeMode::Dark ? kDarkToolbarBtn : kLightToolbarBtn;
}
QColor ThemeManager::toolbarBtnHover() {
    return instance().theme_ == ThemeMode::Dark ? kDarkToolbarBtnHover : kLightToolbarBtnHover;
}

} // namespace QBlock