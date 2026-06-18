set(CMAKE_AUTOMOC ON)
add_library(ui STATIC
    AppController.cpp AppController.h
    MainWindow.cpp MainWindow.h
    SerialController.cpp SerialController.h
    ReplayController.cpp ReplayController.h
    PlotPanel.cpp PlotPanel.h
    View3D.cpp View3D.h
    OrientationPanel.cpp OrientationPanel.h
    MapView.cpp MapView.h
    StatusPanel.cpp StatusPanel.h
    ConsolePanel.cpp ConsolePanel.h
)
target_link_libraries(ui PRIVATE core replay)
target_link_libraries(ui PUBLIC Qt6::Core Qt6::Widgets Qt6::SerialPort Qt6::Charts Qt6::3DCore Qt6::3DRender Qt6::3DExtras)
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
