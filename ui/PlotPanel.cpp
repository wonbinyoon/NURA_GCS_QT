#include "PlotPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

PlotPanel::PlotPanel(QWidget* parent)
    : QWidget(parent) {

    // Initialize Chart
    m_chart = new QChart();
    m_chart->setTitle("Telemetry Data");
    m_chart->legend()->hide();
    m_chart->setAnimationOptions(QChart::NoAnimation); // Important for high FPS

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // Initialize Axes
    m_axisX = new QValueAxis();
    m_axisX->setTitleText("Time (ext seq)");
    m_axisX->setLabelFormat("%d");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("Value");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // Initialize Series
    m_seriesAltitude = new QLineSeries();
    m_seriesAltitude->setName("Altitude");
    m_chart->addSeries(m_seriesAltitude);
    m_seriesAltitude->attachAxis(m_axisX);
    m_seriesAltitude->attachAxis(m_axisY);

    m_seriesVelocityZ = new QLineSeries();
    m_seriesVelocityZ->setName("Velocity Z");
    m_chart->addSeries(m_seriesVelocityZ);
    m_seriesVelocityZ->attachAxis(m_axisX);
    m_seriesVelocityZ->attachAxis(m_axisY);

    m_seriesAccelZ = new QLineSeries();
    m_seriesAccelZ->setName("Accel Z");
    m_chart->addSeries(m_seriesAccelZ);
    m_seriesAccelZ->attachAxis(m_axisX);
    m_seriesAccelZ->attachAxis(m_axisY);

    // Initialize UI Controls
    m_checkAltitude = new QCheckBox("Altitude", this);
    m_checkVelocityZ = new QCheckBox("Velocity Z", this);
    m_checkAccelZ = new QCheckBox("Accel Z", this);

    m_checkAltitude->setChecked(true);
    m_checkVelocityZ->setChecked(true);
    m_checkAccelZ->setChecked(true);

    QVBoxLayout* controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(m_checkAltitude);
    controlsLayout->addWidget(m_checkVelocityZ);
    controlsLayout->addWidget(m_checkAccelZ);
    controlsLayout->addStretch();

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(controlsLayout, 1);
    mainLayout->addWidget(m_chartView, 4);

    // Timer for 30 FPS rendering
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PlotPanel::onTimerTick);
    m_timer->start(33);
}

void PlotPanel::onDataFrameReceived(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffer.push_back(frame);

    // Limit buffer size to 1000 points
    if (m_buffer.size() > 1000) {
        m_buffer.pop_front();
    }
}

void PlotPanel::onTimerTick() {
    QList<QPointF> pointsAltitude;
    QList<QPointF> pointsVelocityZ;
    QList<QPointF> pointsAccelZ;

    double minX = 0;
    double maxX = 0;
    double minY = -10.0; // Default scale bounds
    double maxY = 10.0;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty()) {
            return;
        }

        // Fast paths to avoid re-allocating internally
        pointsAltitude.reserve(m_buffer.size());
        pointsVelocityZ.reserve(m_buffer.size());
        pointsAccelZ.reserve(m_buffer.size());

        minX = m_buffer.front().timestamp_ext;
        maxX = m_buffer.back().timestamp_ext;

        for (const auto& frame : m_buffer) {
            double x = static_cast<double>(frame.timestamp_ext);

            double alt = static_cast<double>(frame.altitude);
            double vz = static_cast<double>(frame.velocity_ned[2]);
            double az = static_cast<double>(frame.acceleration[2]);

            if (m_checkAltitude->isChecked()) {
                pointsAltitude.append(QPointF(x, alt));
                if (alt < minY) minY = alt;
                if (alt > maxY) maxY = alt;
            }
            if (m_checkVelocityZ->isChecked()) {
                pointsVelocityZ.append(QPointF(x, vz));
                if (vz < minY) minY = vz;
                if (vz > maxY) maxY = vz;
            }
            if (m_checkAccelZ->isChecked()) {
                pointsAccelZ.append(QPointF(x, az));
                if (az < minY) minY = az;
                if (az > maxY) maxY = az;
            }
        }
    }

    // Update chart logic
    m_seriesAltitude->setVisible(m_checkAltitude->isChecked());
    m_seriesVelocityZ->setVisible(m_checkVelocityZ->isChecked());
    m_seriesAccelZ->setVisible(m_checkAccelZ->isChecked());

    m_seriesAltitude->replace(pointsAltitude);
    m_seriesVelocityZ->replace(pointsVelocityZ);
    m_seriesAccelZ->replace(pointsAccelZ);

    // Prevent collapsing axes
    if (minX == maxX) maxX = minX + 1;
    if (minY == maxY) maxY = minY + 1;

    // Add margin to Y
    double yMargin = (maxY - minY) * 0.1;

    m_axisX->setRange(minX, maxX);
    m_axisY->setRange(minY - yMargin, maxY + yMargin);
}
