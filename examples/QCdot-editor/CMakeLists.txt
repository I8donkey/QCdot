# QCdot-editor Example - Visual programming editor application
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(QCdot-editor main.cpp NodePalette.h)

target_link_libraries(QCdot-editor PRIVATE
    QBlockEditor
    QBlockEngine
    QBlockNodes
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set output name for the executable
set_target_properties(QCdot-editor PROPERTIES OUTPUT_NAME "QCdot-editor")