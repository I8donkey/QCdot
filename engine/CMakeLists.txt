# QBlock Engine - Header-only core library
add_library(QBlockEngine INTERFACE)

target_include_directories(QBlockEngine INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(QBlockEngine INTERFACE
    Qt6::Core
    Qt6::Gui
)