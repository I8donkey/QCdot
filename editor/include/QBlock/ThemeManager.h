#ifndef QBLOCK_THEME_MANAGER_H
#define QBLOCK_THEME_MANAGER_H

#include <QColor>
#include <QString>
#include <functional>

namespace QBlock {

/// Theme mode for the editor.
enum class ThemeMode {
    Dark,
    Light
};

/// Manages editor color themes (dark/light mode).
class ThemeManager final {
public:
    static ThemeManager& instance();

    ThemeMode currentTheme() const { return theme_; }
    void setTheme(ThemeMode mode);

    /// Register a callback that fires when the theme changes.
    using ThemeCallback = std::function<void()>;
    static void onThemeChanged(ThemeCallback cb);

    // ---- Color accessors ----

    static QColor background();
    static QColor gridMinor();
    static QColor gridMajor();
    static QColor panelBg();
    static QColor panelBorder();
    static QColor textPrimary();
    static QColor textSecondary();
    static QColor nodeBg();
    static QColor nodeBgSelected();
    static QColor nodeBorder();
    static QColor nodeBorderSelected();
    static QColor nodeHeaderFrom();
    static QColor nodeHeaderTo();
    static QColor toolbarBg();
    static QColor toolbarBtn();
    static QColor toolbarBtnHover();

private:
    ThemeManager() = default;
    ThemeMode theme_ = ThemeMode::Dark;
};

} // namespace QBlock

#endif // QBLOCK_THEME_MANAGER_H