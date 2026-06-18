cmake_minimum_required(VERSION 3.16)
project(RocketTelemetry)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Widgets SerialPort Charts 3DCore 3DRender 3DExtras REQUIRED)

add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(simulator)
add_subdirectory(logger)
add_subdirectory(replay)
add_subdirectory(ui)
add_subdirectory(app)

enable_testing()
find_package(GTest REQUIRED)
add_executable(AppControllerTestExec tests/app_controller_test.cpp tests/parser_test.cpp tests/test_pipeline.cpp)
target_link_libraries(AppControllerTestExec PRIVATE ui logger simulator GTest::gtest_main)
target_include_directories(AppControllerTestExec PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME AppControllerTest COMMAND AppControllerTestExec)
