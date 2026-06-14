cmake_minimum_required(VERSION 3.16)
project(AppControllerTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)

add_library(AppControllerLib STATIC
    ui/AppController.cpp
    ui/AppController.h
)

target_link_libraries(AppControllerLib PRIVATE Qt6::Core)
target_include_directories(AppControllerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
find_package(GTest REQUIRED)
add_executable(AppControllerTestExec tests/app_controller_test.cpp)
target_link_libraries(AppControllerTestExec PRIVATE AppControllerLib GTest::gtest_main Qt6::Core)
add_test(NAME AppControllerTest COMMAND AppControllerTestExec)
