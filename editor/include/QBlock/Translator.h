#ifndef QBLOCK_TRANSLATOR_H
#define QBLOCK_TRANSLATOR_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <functional>
#include <vector>

namespace QBlock {

/// Lightweight translation manager (Chinese/English).
/// Uses embedded JSON language data — no external files needed.
class Translator final {
public:
    using Callback = std::function<void()>;

    /// Set active language ("en" or "zh").
    static void setLanguage(const QString& lang);

    /// Get current language code.
    static QString currentLanguage();

    /// Translate key to current language. Returns key itself if not found.
    static QString tr(const QString& key);

    /// Register a callback invoked when language changes (for UI refresh).
    static void onLanguageChanged(Callback cb);

    /// Remove all callbacks.
    static void clearCallbacks();

private:
    static const QHash<QString, QHash<QString, QString>>& data();
    static QString s_currentLang;
    static std::vector<Callback> s_callbacks;
};

} // namespace QBlock

#endif // QBLOCK_TRANSLATOR_H