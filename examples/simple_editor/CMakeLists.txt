find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(simple_editor main.cpp NodePalette.h)

target_link_libraries(simple_editor PRIVATE
    QBlockEditor
    QBlockEngine
    QBlockNodes
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)