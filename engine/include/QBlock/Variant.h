#ifndef QBLOCK_VARIANT_H
#define QBLOCK_VARIANT_H

#include "DataType.h"
#include <QByteArray>
#include <QString>
#include <QVariant>
#include <variant>
#include <string>
#include <vector>

namespace QBlock {

/// Runtime value that can hold any supported data type.
/// Used as the data payload flowing through nodes.
class Variant final {
public:
    // Constructors
    Variant() : type_(DataType::None) {}
    explicit Variant(int64_t v)        : type_(DataType::Integer),  storage_(v) {}
    explicit Variant(double v)         : type_(DataType::Float),    storage_(v) {}
    explicit Variant(bool v)           : type_(DataType::Boolean),  storage_(v) {}
    explicit Variant(QString v)        : type_(DataType::String),   storage_(std::move(v)) {}
    explicit Variant(std::string v)    : type_(DataType::String),   storage_(QString::fromStdString(std::move(v))) {}
    explicit Variant(QByteArray v)     : type_(DataType::Binary),   storage_(std::move(v)) {}
    explicit Variant(std::vector<uint8_t> v)
        : type_(DataType::Binary), storage_(QByteArray(reinterpret_cast<const char*>(v.data()),
                                                       static_cast<int>(v.size()))) {}

    Variant(const Variant&) = default;
    Variant& operator=(const Variant&) = default;
    Variant(Variant&&) = default;
    Variant& operator=(Variant&&) = default;

    // Type access
    DataType type() const { return type_; }
    bool isValid() const { return type_ != DataType::None; }

    // Value accessors
    int64_t  toInt()    const { return holds<int64_t>()  ? get<int64_t>()  : 0; }
    double   toFloat()  const { return holds<double>()   ? get<double>()   : 0.0; }
    bool     toBool()   const { return holds<bool>()     ? get<bool>()     : false; }
    QString  toString() const;
    QByteArray toBinary() const;

    // Convert to QVariant for Qt integration
    QVariant toQVariant() const;

    // Convert from QVariant
    static Variant fromQVariant(const QVariant& v);

    // Equality
    bool operator==(const Variant& other) const;
    bool operator!=(const Variant& other) const { return !(*this == other); }

private:
    using Storage = std::variant<int64_t, double, bool, QString, QByteArray>;

    DataType type_;
    Storage  storage_;

    template <typename T> bool holds() const {
        return std::holds_alternative<T>(storage_);
    }
    template <typename T> const T& get() const {
        return std::get<T>(storage_);
    }
};

// ---- Inline implementations ----

inline QString Variant::toString() const {
    switch (type_) {
        case DataType::Integer:  return QString::number(get<int64_t>());
        case DataType::Float:    return QString::number(get<double>());
        case DataType::Boolean:  return get<bool>() ? QStringLiteral("true") : QStringLiteral("false");
        case DataType::String:   return get<QString>();
        case DataType::Binary:   return QStringLiteral("[Binary: %1 bytes]").arg(get<QByteArray>().size());
        default:                 return {};
    }
}

inline QByteArray Variant::toBinary() const {
    if (type_ == DataType::Binary)
        return get<QByteArray>();
    return {};
}

inline QVariant Variant::toQVariant() const {
    switch (type_) {
        case DataType::Integer:  return QVariant(static_cast<qlonglong>(toInt()));
        case DataType::Float:    return QVariant(toFloat());
        case DataType::Boolean:  return QVariant(toBool());
        case DataType::String:   return QVariant(toString());
        case DataType::Binary:   return QVariant(toBinary());
        default:                 return {};
    }
}

inline Variant Variant::fromQVariant(const QVariant& v) {
    switch (v.typeId()) {
        case QMetaType::Int:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return Variant(v.toLongLong());
        case QMetaType::Double:
        case QMetaType::Float:
            return Variant(v.toDouble());
        case QMetaType::Bool:
            return Variant(v.toBool());
        case QMetaType::QString:
            return Variant(v.toString());
        case QMetaType::QByteArray:
            return Variant(v.toByteArray());
        default:
            return Variant();
    }
}

inline bool Variant::operator==(const Variant& other) const {
    if (type_ != other.type_) return false;
    return storage_ == other.storage_;
}

} // namespace QBlock

#endif // QBLOCK_VARIANT_H