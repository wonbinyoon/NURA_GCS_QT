#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

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

    // Top Controls
    QWidget* topControls = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topControls);
    topLayout->addWidget(m_serialController);
    topLayout->addWidget(m_replayController);
    topLayout->addStretch();

    // Middle Splitter
    QSplitter* middleSplitter = new QSplitter(Qt::Horizontal, this);
    middleSplitter->addWidget(m_plotPanel);
    middleSplitter->addWidget(m_view3D);
    middleSplitter->addWidget(m_orientationPanel);

    // Lower Splitter
    QSplitter* lowerSplitter = new QSplitter(Qt::Horizontal, this);
    lowerSplitter->addWidget(m_mapView);
    lowerSplitter->addWidget(m_statusPanel);

    // Main Splitter
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(topControls);
    mainSplitter->addWidget(middleSplitter);
    mainSplitter->addWidget(lowerSplitter);
    mainSplitter->addWidget(m_consolePanel);

    // Set stretch factors
    mainSplitter->setStretchFactor(0, 0); // Controls (fixed height ideally)
    mainSplitter->setStretchFactor(1, 3); // Main views
    mainSplitter->setStretchFactor(2, 1); // Map & Status
    mainSplitter->setStretchFactor(3, 1); // Console

    setCentralWidget(mainSplitter);
    resize(1400, 900);

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
