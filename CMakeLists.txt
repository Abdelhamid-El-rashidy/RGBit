cmake_minimum_required(VERSION 3.16)

project(IMG-Processer-GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

set(PROJECT_SOURCES
        main.cpp
        pre.cpp
        Filters/Image_Filters.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        imageview.cpp
        imageview.h
)

qt_add_executable(IMG-Processer-GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        assets.qrc
)

target_link_libraries(IMG-Processer-GUI PRIVATE Qt6::Widgets Qt6::Gui)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_resources(RESOURCE_FILES assets.qrc)
target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_FILES})

qt_finalize_executable(IMG-Processer-GUI)
