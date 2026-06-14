#include "ReplayController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ReplayController::ReplayController(QWidget* parent)
    : QWidget(parent) {

    // UI Components
    m_playButton = new QPushButton("Play", this);
    m_pauseButton = new QPushButton("Pause", this);
    m_stopButton = new QPushButton("Stop", this);

    m_speedSpinBox = new QDoubleSpinBox(this);
    m_speedSpinBox->setRange(0.1, 10.0);
    m_speedSpinBox->setSingleStep(0.1);
    m_speedSpinBox->setValue(1.0);

    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setEnabled(false); // Disabled until range is set by backend

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(m_playButton);
    controlsLayout->addWidget(m_pauseButton);
    controlsLayout->addWidget(m_stopButton);

    controlsLayout->addWidget(new QLabel("Speed:", this));
    controlsLayout->addWidget(m_speedSpinBox);
    controlsLayout->addStretch();

    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(m_seekSlider);

    // Connections
    connect(m_playButton, &QPushButton::clicked, this, &ReplayController::onPlayClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &ReplayController::onPauseClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &ReplayController::onStopClicked);
    connect(m_seekSlider, &QSlider::sliderMoved, this, &ReplayController::onSliderMoved);
    connect(m_speedSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ReplayController::onSpeedValueChanged);
}

void ReplayController::setSliderRange(int min, int max) {
    m_seekSlider->setRange(min, max);
    m_seekSlider->setEnabled(max > min);
}

void ReplayController::setSliderPosition(int position) {
    bool oldState = m_seekSlider->blockSignals(true);
    m_seekSlider->setValue(position);
    m_seekSlider->blockSignals(oldState);
}

void ReplayController::onPlayClicked() {
    emit playRequested();
}

void ReplayController::onPauseClicked() {
    emit pauseRequested();
}

void ReplayController::onStopClicked() {
    emit stopRequested();
}

void ReplayController::onSliderMoved(int value) {
    emit seekRequested(value);
}

void ReplayController::onSpeedValueChanged(double value) {
    emit speedChanged(value);
}
