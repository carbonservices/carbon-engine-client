cmake_minimum_required(VERSION 3.20)
project(carbon-engine-client VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_package(Qt6 6.10 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Network
    WebEngineQuick
)
qt_standard_project_setup()

# Logging subsystem
add_library(carbon-engine-logging STATIC
    src/logging/LogLevel.h
    src/logging/Logger.h
    src/logging/Logger.cpp
)
target_include_directories(carbon-engine-logging PUBLIC
    src
)
target_link_libraries(carbon-engine-logging PUBLIC
    Qt6::Core
)

# Executable target
set(CLIENT_SOURCES
    src/main.cpp
)
if(WIN32)
    list(APPEND CLIENT_SOURCES app.rc)
endif()

qt_add_executable(carbon-engine-client ${CLIENT_SOURCES})

target_include_directories(carbon-engine-client PRIVATE
    src
    src/api
    src/common
    src/logging
)

target_link_libraries(carbon-engine-client PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebEngineQuick
    carbon-engine-logging
)

if(WIN32)
    set_target_properties(carbon-engine-client PROPERTIES
        WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
    )
elseif(UNIX AND NOT APPLE)
    set_target_properties(carbon-engine-client PROPERTIES
        INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

add_custom_command(TARGET carbon-engine-client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/logo.png"
        $<TARGET_FILE_DIR:carbon-engine-client>/logo.png
)

set(QML_FILES
    src/qml/Main.qml
    src/qml/Theme.qml
    src/qml/LeftSidebar.qml
    src/qml/CentralWorkspace.qml
    src/qml/RightPanel.qml
    src/qml/DocumentsPanel.qml
    src/qml/SystemStatusPanel.qml
    src/qml/StatusLogPanel.qml
    src/qml/ReportReviewScreen.qml
    src/qml/FieldGroupCard.qml
    src/qml/LoopFieldEditor.qml
    src/qml/SettingsDialog.qml
    src/qml/RateLimitDialog.qml
    src/qml/BRSRMetadata.js
)

foreach(qml_file IN LISTS QML_FILES)
    get_filename_component(filename ${qml_file} NAME)
    set_source_files_properties(${qml_file} PROPERTIES QT_RESOURCE_ALIAS ${filename})
endforeach()

set_source_files_properties(src/qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(logo.png PROPERTIES QT_RESOURCE_ALIAS logo.png)

qt_add_qml_module(carbon-engine-client
    URI CarbonEngineClient
    VERSION 1.0
    QML_FILES ${QML_FILES}
    RESOURCES logo.png
    SOURCES
        src/api/ApiClientBase.h src/api/ApiClientBase.cpp
        src/api/UploadClient.h src/api/UploadClient.cpp
        src/api/PipelineStatusPoller.h src/api/PipelineStatusPoller.cpp
        src/api/ReportClient.h src/api/ReportClient.cpp
        src/api/SystemStatusPoller.h src/api/SystemStatusPoller.cpp
        src/api/ExtractionClient.h src/api/ExtractionClient.cpp
        src/api/ExtractionStatusPoller.h src/api/ExtractionStatusPoller.cpp
        src/api/ModeClient.h src/api/ModeClient.cpp
)

