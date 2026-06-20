# QBlock Editor - Qt6 graphical node editor library
add_library(QBlockEditor STATIC
    include/QBlock/PortWidget.h
    include/QBlock/ConnectionWidget.h
    include/QBlock/NodeWidget.h
    include/QBlock/EditorScene.h
    include/QBlock/NodeEditor.h
    include/QBlock/Translator.h
    include/QBlock/ThemeManager.h
    include/QBlock/NodePickerDialog.h
    include/QBlock/InlineNodePicker.h
    src/PortWidget.cpp
    src/ConnectionWidget.cpp
    src/NodeWidget.cpp
    src/EditorScene.cpp
    src/NodeEditor.cpp
    src/Translator.cpp
    src/ThemeManager.cpp
    src/NodePickerDialog.cpp
)

target_include_directories(QBlockEditor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(QBlockEditor PUBLIC
    QBlockEngine
    QBlockNodes
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)