#include "MainWindow.h"
#include <QVBoxLayout>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    // Initialize AppController
    m_appController = new AppController(this);

    // Initialize UI components
    m_serialController = new SerialController(this);
    m_replayController = new ReplayController(this);
    m_statusPanel = new StatusPanel(this);

    m_plotPanel = new PlotPanel(this);
    m_view3D = new View3D(this);
    m_orientationPanel = new OrientationPanel(this);
    m_mapView = new MapView(this);

    m_consolePanel = new ConsolePanel(this);

    // Left Control Panel Layout
    QWidget* controlPanelWidget = new QWidget(this);
    QVBoxLayout* controlPanelLayout = new QVBoxLayout(controlPanelWidget);
    controlPanelLayout->addWidget(m_serialController);
    controlPanelLayout->addWidget(m_replayController);
    controlPanelLayout->addWidget(m_statusPanel);
    controlPanelLayout->addStretch();

    // Right Visualization Area Layout
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(m_plotPanel, "Plots");
    m_tabWidget->addTab(m_view3D, "3D View");
    m_tabWidget->addTab(m_orientationPanel, "Orientation");
    m_tabWidget->addTab(m_mapView, "Map");

    // Bottom Console Layout
    QWidget* consoleWidget = new QWidget(this);
    QVBoxLayout* consoleLayout = new QVBoxLayout(consoleWidget);
    consoleLayout->addWidget(m_consolePanel);

    // Set up splitters
    QSplitter* topSplitter = new QSplitter(Qt::Horizontal, this);
    topSplitter->addWidget(controlPanelWidget);
    topSplitter->addWidget(m_tabWidget);
    topSplitter->setStretchFactor(0, 1);
    topSplitter->setStretchFactor(1, 4);

    QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(consoleWidget);
    mainSplitter->setStretchFactor(0, 4);
    mainSplitter->setStretchFactor(1, 1);

    setCentralWidget(mainSplitter);
    resize(1200, 800);

    // Connect Data Distribution
    connect(m_appController, &AppController::dataUpdated, m_plotPanel, &PlotPanel::onDataFrameReceived);
    connect(m_appController, &AppController::dataUpdated, m_view3D, &View3D::onDataFrameReceived);
    connect(m_appController, &AppController::dataUpdated, m_orientationPanel, &OrientationPanel::onDataFrameReceived);
    connect(m_appController, &AppController::dataUpdated, m_mapView, &MapView::onDataFrameReceived);
    connect(m_appController, &AppController::dataUpdated, m_statusPanel, &StatusPanel::onDataFrameReceived);

    // Connect Serial Controller to AppController
    connect(m_serialController, &SerialController::connectRequested, m_appController, &AppController::connectSerial);
    connect(m_serialController, &SerialController::disconnectRequested, m_appController, &AppController::disconnectSerial);

    // Connect Replay Controller to AppController
    connect(m_replayController, &ReplayController::playRequested, m_appController, &AppController::playReplay);
    connect(m_replayController, &ReplayController::pauseRequested, m_appController, &AppController::pauseReplay);
    connect(m_replayController, &ReplayController::stopRequested, m_appController, &AppController::stopReplay);
    connect(m_replayController, &ReplayController::seekRequested, m_appController, &AppController::seekReplay);
    connect(m_replayController, &ReplayController::speedChanged, m_appController, &AppController::setReplaySpeed);

    // Connect AppController logs to ConsolePanel
    connect(m_appController, &AppController::logMessage, m_consolePanel, &ConsolePanel::logMessage);
}
