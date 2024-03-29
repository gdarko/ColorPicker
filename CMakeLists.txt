# Reference: https://doc.qt.io/qt-6/cmake-manual.html

cmake_minimum_required(VERSION 3.16)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Qt basic functions
include(QtCommon)

# Basic information about project

project(ColorPicker)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

# Set additional project information
set(COMPANY "IDEOLOGIX Media")
set(COPYRIGHT "Copyright (c) 2022 Darko Gjorgjijoski. All rights reserved.")
set(IDENTIFIER "com.ideologix.colorpicker")


if (UNIX AND NOT APPLE)
    set(SOURCE_FILES
            src/main.cpp
            src/mainwindow.cpp
            src/pointercolor.cpp
            src/qguiappcurrentscreen.cpp
            src/desktopinfo.cpp
            src/screengrabber.cpp
            src/dialogabout.cpp
            src/dialogstartupinfo.cpp
            src/request.cpp
            )
else ()
    set(SOURCE_FILES
            src/main.cpp
            src/mainwindow.cpp
            src/pointercolor.cpp
            src/qguiappcurrentscreen.cpp
            src/desktopinfo.cpp
            src/screengrabber.cpp
            src/dialogabout.cpp
            src/dialogstartupinfo.cpp
            )
endif ()


add_project_meta(META_FILES_TO_INCLUDE)

set(RESOURCE_FILES res/assets.qrc)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS DBus REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# source_group("UI Files" FILES ${UI_FILES})

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
        ${SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
        )

target_precompile_headers(${PROJECT_NAME} INTERFACE QtWidgets.h)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::DBus)


if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ColorPicker)
endif ()


