add_library(core STATIC RocketTelemetryParser.cpp RocketTelemetryParser.h IProtocolParser.h)
target_link_libraries(core PUBLIC core_model)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
