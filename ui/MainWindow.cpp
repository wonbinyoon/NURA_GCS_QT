#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    // Create the three main components
    m_controlPanel = new QWidget(this);
    m_visualizationArea = new QWidget(this);
    m_console = new QWidget(this);

    // For visualization purposes before actual components are added, we can set some minimum sizes
    m_controlPanel->setMinimumSize(200, 200);
    m_visualizationArea->setMinimumSize(400, 200);
    m_console->setMinimumSize(600, 100);

    // Create a horizontal splitter for the top part (Control Panel on left, Visualization Area on right)
    QSplitter* topSplitter = new QSplitter(Qt::Horizontal, this);
    topSplitter->addWidget(m_controlPanel);
    topSplitter->addWidget(m_visualizationArea);
    // Set stretch factors: Visualization Area should take up more space
    topSplitter->setStretchFactor(0, 1);
    topSplitter->setStretchFactor(1, 4);

    // Create a vertical splitter for the main layout (Top part above, Console below)
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(m_console);
    // Set stretch factors: Top part should take up more space than console
    mainSplitter->setStretchFactor(0, 4);
    mainSplitter->setStretchFactor(1, 1);

    // Set the main splitter as the central widget
    setCentralWidget(mainSplitter);

    // Set an initial size
    resize(800, 600);
}
