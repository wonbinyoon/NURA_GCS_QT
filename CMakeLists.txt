cmake_minimum_required(VERSION 3.16)
project(AppControllerTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Widgets SerialPort Charts 3DCore 3DRender 3DExtras REQUIRED)

set(CMAKE_AUTOMOC ON)

add_library(AppControllerLib STATIC
    ui/AppController.cpp
    ui/AppController.h
    ui/MainWindow.cpp
    ui/MainWindow.h
    ui/SerialController.cpp
    ui/SerialController.h
    ui/ReplayController.cpp
    ui/ReplayController.h
    ui/PlotPanel.cpp
    ui/PlotPanel.h
    ui/View3D.cpp
    ui/View3D.h
    ui/OrientationPanel.cpp
    ui/OrientationPanel.h
)

target_link_libraries(AppControllerLib PRIVATE Qt6::Core Qt6::Widgets Qt6::SerialPort Qt6::Charts Qt6::3DCore Qt6::3DRender Qt6::3DExtras)
target_include_directories(AppControllerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
find_package(GTest REQUIRED)
add_executable(AppControllerTestExec tests/app_controller_test.cpp)
target_link_libraries(AppControllerTestExec PRIVATE AppControllerLib GTest::gtest_main Qt6::Core Qt6::Widgets Qt6::SerialPort Qt6::Charts Qt6::3DCore Qt6::3DRender Qt6::3DExtras)
add_test(NAME AppControllerTest COMMAND AppControllerTestExec)
