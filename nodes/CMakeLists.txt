# QBlock Builtin Nodes - Header-only library
add_library(QBlockNodes INTERFACE)

target_include_directories(QBlockNodes INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(QBlockNodes INTERFACE
    QBlockEngine
)