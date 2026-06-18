add_library(logger STATIC DataLogger.cpp DataLogger.h)
target_link_libraries(logger PRIVATE core_model)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
