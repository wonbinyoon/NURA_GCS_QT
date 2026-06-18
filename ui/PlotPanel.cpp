#include <QLabel>
#include "PlotPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

PlotPanel::PlotPanel(QWidget* parent)
    : QWidget(parent) {

    QWidget* scrollWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(scrollWidget);

    setupGroup(m_posAltGroup, "Position / Altitude", {"North", "East", "Down", "Altitude"});
    setupGroup(m_velGroup, "Velocity", {"Vx", "Vy", "Vz"});
    setupGroup(m_accGroup, "Acceleration", {"Ax", "Ay", "Az"});
    setupGroup(m_imuGroup, "IMU", {"GyroX", "GyroY", "GyroZ", "MagX", "MagY", "MagZ"});
    setupGroup(m_envGroup, "Environment", {"Pressure", "Temperature"});

    mainLayout->addWidget(m_posAltGroup.view);
    mainLayout->addWidget(m_velGroup.view);
    mainLayout->addWidget(m_accGroup.view);
    mainLayout->addWidget(m_imuGroup.view);
    mainLayout->addWidget(m_envGroup.view);

    // Add checkboxes on the left
    QVBoxLayout* checkLayout = new QVBoxLayout();
    auto addChecks = [&](PlotGroup& group, const QString& title) {
        checkLayout->addWidget(new QLabel(title));
        for (auto* cb : group.checkboxes) checkLayout->addWidget(cb);
    };

    addChecks(m_posAltGroup, "Pos/Alt");
    addChecks(m_velGroup, "Velocity");
    addChecks(m_accGroup, "Acceleration");
    addChecks(m_imuGroup, "IMU");
    addChecks(m_envGroup, "Environment");
    checkLayout->addStretch();

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    QHBoxLayout* rootLayout = new QHBoxLayout(this);
    rootLayout->addLayout(checkLayout, 1);
    rootLayout->addWidget(scrollArea, 4);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PlotPanel::onTimerTick);
    m_timer->start(33);
}

void PlotPanel::setupGroup(PlotGroup& group, const QString& title, const QStringList& seriesNames) {
    group.chart = new QChart();
    group.chart->setTitle(title);
    group.chart->setAnimationOptions(QChart::NoAnimation);

    group.view = new QChartView(group.chart);
    group.view->setRenderHint(QPainter::Antialiasing);
    group.view->setMinimumHeight(250);

    group.axisX = new QValueAxis();
    group.axisX->setTitleText("Time");
    group.chart->addAxis(group.axisX, Qt::AlignBottom);

    group.axisY = new QValueAxis();
    group.axisY->setTitleText("Value");
    group.chart->addAxis(group.axisY, Qt::AlignLeft);

    for (const QString& name : seriesNames) {
        auto* series = new QLineSeries();
        series->setName(name);
        group.chart->addSeries(series);
        series->attachAxis(group.axisX);
        series->attachAxis(group.axisY);
        group.series.append(series);

        auto* cb = new QCheckBox(name);
        cb->setChecked(true);
        group.checkboxes.append(cb);
    }
}

void PlotPanel::onDataFrameReceived(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffer.push_back(frame);
    if (m_buffer.size() > 500) {
        m_buffer.pop_front();
    }
}

void PlotPanel::onTimerTick() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_buffer.empty()) return;

    double minX = m_buffer.front().timestamp_ext;
    double maxX = m_buffer.back().timestamp_ext;

    QList<QList<QPointF>> posAltData(4), velData(3), accData(3), imuData(6), envData(2);
    double py[5][2] = {{1e9, -1e9}, {1e9, -1e9}, {1e9, -1e9}, {1e9, -1e9}, {1e9, -1e9}};

    for (const auto& f : m_buffer) {
        double x = f.timestamp_ext;

        // Pos/Alt
        if (m_posAltGroup.checkboxes[0]->isChecked()) { posAltData[0].append(QPointF(x, f.position_ned[0])); py[0][0] = std::min(py[0][0], (double)f.position_ned[0]); py[0][1] = std::max(py[0][1], (double)f.position_ned[0]); }
        if (m_posAltGroup.checkboxes[1]->isChecked()) { posAltData[1].append(QPointF(x, f.position_ned[1])); py[0][0] = std::min(py[0][0], (double)f.position_ned[1]); py[0][1] = std::max(py[0][1], (double)f.position_ned[1]); }
        if (m_posAltGroup.checkboxes[2]->isChecked()) { posAltData[2].append(QPointF(x, f.position_ned[2])); py[0][0] = std::min(py[0][0], (double)f.position_ned[2]); py[0][1] = std::max(py[0][1], (double)f.position_ned[2]); }
        if (m_posAltGroup.checkboxes[3]->isChecked()) { posAltData[3].append(QPointF(x, f.altitude)); py[0][0] = std::min(py[0][0], (double)f.altitude); py[0][1] = std::max(py[0][1], (double)f.altitude); }

        // Vel
        for(int i=0; i<3; ++i) if (m_velGroup.checkboxes[i]->isChecked()) { velData[i].append(QPointF(x, f.velocity_ned[i])); py[1][0] = std::min(py[1][0], (double)f.velocity_ned[i]); py[1][1] = std::max(py[1][1], (double)f.velocity_ned[i]); }

        // Acc
        for(int i=0; i<3; ++i) if (m_accGroup.checkboxes[i]->isChecked()) { accData[i].append(QPointF(x, f.acceleration[i])); py[2][0] = std::min(py[2][0], (double)f.acceleration[i]); py[2][1] = std::max(py[2][1], (double)f.acceleration[i]); }

        // IMU
        for(int i=0; i<3; ++i) if (m_imuGroup.checkboxes[i]->isChecked()) { imuData[i].append(QPointF(x, f.gyro[i])); py[3][0] = std::min(py[3][0], (double)f.gyro[i]); py[3][1] = std::max(py[3][1], (double)f.gyro[i]); }
        for(int i=0; i<3; ++i) if (m_imuGroup.checkboxes[i+3]->isChecked()) { imuData[i+3].append(QPointF(x, f.mag[i])); py[3][0] = std::min(py[3][0], (double)f.mag[i]); py[3][1] = std::max(py[3][1], (double)f.mag[i]); }

        // Env
        if (m_envGroup.checkboxes[0]->isChecked()) { envData[0].append(QPointF(x, f.pressure)); py[4][0] = std::min(py[4][0], (double)f.pressure); py[4][1] = std::max(py[4][1], (double)f.pressure); }
        if (m_envGroup.checkboxes[1]->isChecked()) { envData[1].append(QPointF(x, f.temperature)); py[4][0] = std::min(py[4][0], (double)f.temperature); py[4][1] = std::max(py[4][1], (double)f.temperature); }
    }

    updateGroup(m_posAltGroup, posAltData, minX, maxX, py[0][0], py[0][1]);
    updateGroup(m_velGroup, velData, minX, maxX, py[1][0], py[1][1]);
    updateGroup(m_accGroup, accData, minX, maxX, py[2][0], py[2][1]);
    updateGroup(m_imuGroup, imuData, minX, maxX, py[3][0], py[3][1]);
    updateGroup(m_envGroup, envData, minX, maxX, py[4][0], py[4][1]);
}

void PlotPanel::updateGroup(PlotGroup& group, const QList<QList<QPointF>>& data, double minX, double maxX, double minY, double maxY) {
    for (int i = 0; i < group.series.size(); ++i) {
        group.series[i]->setVisible(group.checkboxes[i]->isChecked());
        if (group.checkboxes[i]->isChecked()) {
            group.series[i]->replace(data[i]);
        }
    }

    if (minX == maxX) maxX = minX + 1;
    if (minY >= maxY) { minY = -1; maxY = 1; }
    double marginY = (maxY - minY) * 0.1;

    group.axisX->setRange(minX, maxX);
    group.axisY->setRange(minY - marginY, maxY + marginY);
}
