add_library(simulator STATIC TelemetrySimulator.cpp TelemetrySimulator.h)
target_include_directories(simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
