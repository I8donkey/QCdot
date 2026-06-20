# QCdot-editor Example - Visual programming editor application
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(QCdot-editor WIN32 main.cpp NodePalette.h)

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

# Deploy Qt dependencies automatically after build
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET QCdot-editor POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-compiler-runtime
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:QCdot-editor>"
            COMMENT "Deploying Qt dependencies..."
        )
    endif()
endif()