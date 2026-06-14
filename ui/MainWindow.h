#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

#include "AppController.h"
#include "SerialController.h"
#include "ReplayController.h"
#include "PlotPanel.h"
#include "View3D.h"
#include "OrientationPanel.h"
#include "MapView.h"
#include "StatusPanel.h"
#include "ConsolePanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    AppController* m_appController;

    SerialController* m_serialController;
    ReplayController* m_replayController;
    StatusPanel* m_statusPanel;

    PlotPanel* m_plotPanel;
    View3D* m_view3D;
    OrientationPanel* m_orientationPanel;
    MapView* m_mapView;
    QTabWidget* m_tabWidget;

    ConsolePanel* m_consolePanel;
};

#endif // MAINWINDOW_H
