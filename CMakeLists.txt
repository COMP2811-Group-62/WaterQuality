cmake_minimum_required(VERSION 3.16)
project(cwk2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/Users/alexandermccall/Qt/6.8.0/macos/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Quick QuickWidgets)
qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/navigationbar.cpp
    src/basepage.cpp
    src/dashboardpage.cpp
    src/datapage.cpp
    src/trendsoverview.cpp
    src/fluorinatedcompounds.cpp
    src/styles.cpp
    src/qualitysample.cpp
    src/dataset.cpp
    src/model.cpp
    src/popspage.cpp
    src/litterpage.cpp
    src/compliancedashboard.cpp
)

set(HEADERS
    src/mainwindow.h
    src/navigationbar.h
    src/basepage.h
    src/dashboardpage.h
    src/datapage.h
    src/trendsoverview.h
    src/fluorinatedcompounds.h
    src/styles.h
    src/qualitysample.h
    src/dataset.h
    src/model.h
    src/litterpage.h
    src/popspage.h
    src/compliancedashboard.h
)

qt_add_executable(water
    ${SOURCES}
    ${HEADERS}
    src/resources.qrc
)

target_include_directories(water PRIVATE src)

target_link_libraries(water PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Quick
    Qt6::QuickWidgets
    # Qt6::Location
)

set_target_properties(water PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE OFF
)