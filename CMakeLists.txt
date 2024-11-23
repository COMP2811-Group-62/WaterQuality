cmake_minimum_required(VERSION 3.16)

project(cwk2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/navigationbar.cpp
    src/basepage.cpp
    src/dashboardpage.cpp
    src/examplepage.cpp
    src/litterpage.cpp    # 添加新文件
)

qt_add_executable(water ${SOURCES})

target_include_directories(water PRIVATE src)

target_link_libraries(water PRIVATE Qt6::Widgets)

set_target_properties(water PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE OFF
)