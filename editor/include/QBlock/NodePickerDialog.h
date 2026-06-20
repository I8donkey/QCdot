#ifndef QBLOCK_NODE_PICKER_DIALOG_H
#define QBLOCK_NODE_PICKER_DIALOG_H

#include <QBlock/DataType.h>
#include <QDialog>
#include <QListWidget>
#include <QString>
#include <vector>
#include <functional>

class QLabel;
class QPushButton;

namespace QBlock {

/// A dialog that appears when dragging a connection to a blank area.
/// Shows a list of compatible nodes that can be created and auto-connected.
class NodePickerDialog final : public QDialog {
    Q_OBJECT
public:
    /// @param sourceType  The data type of the port being dragged from
    /// @param isInput     True if we need nodes that accept this type as input
    /// @param nodeFactory Function that returns all available type names
    explicit NodePickerDialog(DataType sourceType, bool isInput,
                              std::function<std::vector<std::string>()> typeLister,
                              QWidget* parent = nullptr);

    /// Returns the selected node type name (empty if cancelled).
    std::string selectedType() const { return selectedType_; }

private:
    void populateList(DataType sourceType, bool isInput,
                      std::function<std::vector<std::string>()> typeLister);
    void applyThemeStyle();

    std::string selectedType_;
    QListWidget* list_ = nullptr;
    QLabel* promptLabel_ = nullptr;
    QPushButton* cancelBtn_ = nullptr;
};

} // namespace QBlock

#endif // QBLOCK_NODE_PICKER_DIALOG_H