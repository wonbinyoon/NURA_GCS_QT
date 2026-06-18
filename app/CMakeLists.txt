add_executable(app_main main.cpp)
target_link_libraries(app_main PRIVATE ui logger simulator)
target_include_directories(app_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
