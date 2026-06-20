#ifndef QBLOCK_DATATYPE_H
#define QBLOCK_DATATYPE_H

#include <QColor>
#include <QString>
#include <cstdint>

namespace QBlock {

/// Data types supported by the static type system.
/// Each type has an associated color for visual identification.
enum class DataType : uint8_t {
    None     = 0,
    Integer  = 1,
    Float    = 2,
    Boolean  = 3,
    String   = 4,
    Binary   = 5,   ///< byte array / binary blob
    FilePath = 6,   ///< file system path
    Generic  = 7,   ///< template / any-type (limited use)
    ExecFlow = 8    ///< execution flow (signal mode only)
};

/// Returns the display color for a given data type.
inline QColor colorForType(DataType type) {
    switch (type) {
        case DataType::None:     return QColor(128, 128, 128);   // Gray
        case DataType::Integer:  return QColor(70,  130, 180);   // Steel Blue
        case DataType::Float:    return QColor(60,  179, 113);   // Medium Sea Green
        case DataType::Boolean:  return QColor(218, 165, 32);    // Goldenrod
        case DataType::String:   return QColor(205, 92,  92);    // Indian Red
        case DataType::Binary:   return QColor(147, 112, 219);   // Medium Purple
        case DataType::FilePath: return QColor(255, 160, 122);   // Light Salmon
        case DataType::Generic:  return QColor(169, 169, 169);   // Dark Gray
        case DataType::ExecFlow: return QColor(255, 255, 255);   // White
    }
    return QColor(128, 128, 128);
}

/// Returns a human-readable name for a data type.
inline QString typeName(DataType type) {
    switch (type) {
        case DataType::None:     return QStringLiteral("None");
        case DataType::Integer:  return QStringLiteral("Integer");
        case DataType::Float:    return QStringLiteral("Float");
        case DataType::Boolean:  return QStringLiteral("Boolean");
        case DataType::String:   return QStringLiteral("String");
        case DataType::Binary:   return QStringLiteral("Binary");
        case DataType::FilePath: return QStringLiteral("FilePath");
        case DataType::Generic:  return QStringLiteral("Generic");
        case DataType::ExecFlow: return QStringLiteral("Exec");
    }
    return QStringLiteral("Unknown");
}

/// Check whether two data types are compatible for connection.
/// Numeric types (Integer, Float, Boolean) are mutually compatible.
inline bool typesCompatible(DataType a, DataType b) {
    if (a == DataType::Generic || b == DataType::Generic)
        return true;
    if (a == b)
        return true;
    // Numeric promotion: Integer, Float, Boolean are mutually compatible
    bool aNumeric = (a == DataType::Integer || a == DataType::Float || a == DataType::Boolean);
    bool bNumeric = (b == DataType::Integer || b == DataType::Float || b == DataType::Boolean);
    if (aNumeric && bNumeric)
        return true;
    return false;
}

} // namespace QBlock

#endif // QBLOCK_DATATYPE_H