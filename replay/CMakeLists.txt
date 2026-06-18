add_library(replay STATIC ReplayStorage.cpp ReplayStorage.h)
target_link_libraries(replay PRIVATE core_model)
target_include_directories(replay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
