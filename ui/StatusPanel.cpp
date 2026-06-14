#include "StatusPanel.h"
#include <QFormLayout>

StatusPanel::StatusPanel(QWidget* parent)
    : QWidget(parent), m_hasData(false) {

    m_fsmLabel = new QLabel("N/A", this);
    m_altLabel = new QLabel("N/A", this);
    m_velLabel = new QLabel("N/A", this);
    m_accLabel = new QLabel("N/A", this);
    m_gpsLabel = new QLabel("N/A", this);

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow("FSM State:", m_fsmLabel);
    layout->addRow("Altitude:", m_altLabel);
    layout->addRow("Velocity (NED):", m_velLabel);
    layout->addRow("Acceleration:", m_accLabel);
    layout->addRow("GPS:", m_gpsLabel);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &StatusPanel::onTimerTick);
    m_timer->start(33);
}

void StatusPanel::onDataFrameReceived(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_latestFrame = frame;
    m_hasData = true;
}

void StatusPanel::onTimerTick() {
    DataFrame frame;
    bool hasData = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_hasData) {
            frame = m_latestFrame;
            hasData = true;
        }
    }

    if (hasData) {
        m_fsmLabel->setText(QString::number(frame.fsm_state));
        m_altLabel->setText(QString::asprintf("%.2f m", frame.altitude));
        m_velLabel->setText(QString::asprintf("[%.2f, %.2f, %.2f] m/s", frame.velocity_ned[0], frame.velocity_ned[1], frame.velocity_ned[2]));
        m_accLabel->setText(QString::asprintf("[%.2f, %.2f, %.2f] m/s²", frame.acceleration[0], frame.acceleration[1], frame.acceleration[2]));
        m_gpsLabel->setText(QString::asprintf("%.6f, %.6f", frame.latitude, frame.longitude));

        if (frame.fsm_state != 0) {
            m_fsmLabel->setStyleSheet("QLabel { color : red; font-weight: bold; }");
        } else {
            m_fsmLabel->setStyleSheet("");
        }
    }
}
