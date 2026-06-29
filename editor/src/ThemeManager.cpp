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

// ---- One Dark inspired theme colors ----
namespace {
    constexpr auto kDarkBg              = QColor(40, 44, 52);
    constexpr auto kDarkGridMinor       = QColor(52, 58, 68);
    constexpr auto kDarkGridMajor       = QColor(64, 71, 83);
    constexpr auto kDarkPanelBg         = QColor(33, 37, 43);
    constexpr auto kDarkPanelBorder     = QColor(52, 58, 68);
    constexpr auto kDarkTextPrimary     = QColor(171, 178, 191);
    constexpr auto kDarkTextSecondary   = QColor(92, 99, 112);
    constexpr auto kDarkTextDim         = QColor(76, 83, 96);
    constexpr auto kDarkNodeBg          = QColor(40, 44, 52);
    constexpr auto kDarkNodeBgSelected  = QColor(62, 68, 81);
    constexpr auto kDarkNodeBorder      = QColor(52, 58, 68);
    constexpr auto kDarkNodeBorderSel   = QColor(97, 175, 239);
    constexpr auto kDarkNodeHdrFrom     = QColor(52, 58, 68);
    constexpr auto kDarkNodeHdrTo       = QColor(40, 44, 52);
    constexpr auto kDarkToolbarBg       = QColor(33, 37, 43);
    constexpr auto kDarkToolbarBtn      = QColor(52, 58, 68);
    constexpr auto kDarkToolbarBtnHover = QColor(64, 71, 83);
    constexpr auto kDarkDialogBg        = QColor(33, 37, 43);
    constexpr auto kDarkDialogBorder    = QColor(52, 58, 68);
    constexpr auto kDarkListBg          = QColor(33, 37, 43);
    constexpr auto kDarkListItemHover   = QColor(46, 52, 64);
    constexpr auto kDarkListSelected    = QColor(62, 68, 81);
    constexpr auto kDarkConnectionLine  = QColor(92, 99, 112);
    constexpr auto kDarkShadow          = QColor(0, 0, 0, 40);

    constexpr auto kLightBg             = QColor(245, 247, 250);
    constexpr auto kLightGridMinor      = QColor(228, 231, 238);
    constexpr auto kLightGridMajor      = QColor(209, 213, 221);
    constexpr auto kLightPanelBg        = QColor(255, 255, 255);
    constexpr auto kLightPanelBorder    = QColor(209, 213, 221);
    constexpr auto kLightTextPrimary    = QColor(38, 42, 50);
    constexpr auto kLightTextSecondary  = QColor(100, 107, 120);
    constexpr auto kLightTextDim        = QColor(156, 163, 175);
    constexpr auto kLightNodeBg         = QColor(255, 255, 255);
    constexpr auto kLightNodeBgSelected = QColor(219, 233, 254);
    constexpr auto kLightNodeBorder     = QColor(209, 213, 221);
    constexpr auto kLightNodeBorderSel  = QColor(59, 130, 246);
    constexpr auto kLightNodeHdrFrom    = QColor(240, 243, 248);
    constexpr auto kLightNodeHdrTo      = QColor(250, 251, 253);
    constexpr auto kLightToolbarBg      = QColor(245, 247, 250);
    constexpr auto kLightToolbarBtn     = QColor(228, 231, 238);
    constexpr auto kLightToolbarBtnHover= QColor(209, 213, 221);
    constexpr auto kLightDialogBg       = QColor(255, 255, 255);
    constexpr auto kLightDialogBorder   = QColor(209, 213, 221);
    constexpr auto kLightListBg         = QColor(250, 251, 253);
    constexpr auto kLightListItemHover  = QColor(239, 242, 247);
    constexpr auto kLightListSelected   = QColor(219, 233, 254);
    constexpr auto kLightConnectionLine = QColor(100, 107, 120);
    constexpr auto kLightShadow         = QColor(0, 0, 0, 15);
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
QColor ThemeManager::textDim() {
    return instance().theme_ == ThemeMode::Dark ? kDarkTextDim : kLightTextDim;
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

QColor ThemeManager::dialogBg() {
    return instance().theme_ == ThemeMode::Dark ? kDarkDialogBg : kLightDialogBg;
}

QColor ThemeManager::dialogBorder() {
    return instance().theme_ == ThemeMode::Dark ? kDarkDialogBorder : kLightDialogBorder;
}

QColor ThemeManager::listBg() {
    return instance().theme_ == ThemeMode::Dark ? kDarkListBg : kLightListBg;
}

QColor ThemeManager::listItemHover() {
    return instance().theme_ == ThemeMode::Dark ? kDarkListItemHover : kLightListItemHover;
}

QColor ThemeManager::listItemSelected() {
    return instance().theme_ == ThemeMode::Dark ? kDarkListSelected : kLightListSelected;
}

QColor ThemeManager::connectionLine() {
    return instance().theme_ == ThemeMode::Dark ? kDarkConnectionLine : kLightConnectionLine;
}

QColor ThemeManager::accentColor() {
    return instance().theme_ == ThemeMode::Dark ? kDarkNodeBorderSel : kLightNodeBorderSel;
}

QColor ThemeManager::shadow() {
    return instance().theme_ == ThemeMode::Dark ? kDarkShadow : kLightShadow;
}

} // namespace QBlock