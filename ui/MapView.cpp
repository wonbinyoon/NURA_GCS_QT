#include "MapView.h"
#include <QVBoxLayout>

MapView::MapView(QWidget* parent)
    : QWidget(parent), m_hasData(false) {

    // Initialize Chart
    m_chart = new QChart();
    m_chart->setTitle("GPS Trajectory (2D Map)");
    m_chart->legend()->hide();
    m_chart->setAnimationOptions(QChart::NoAnimation);

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // Initialize Axes
    m_axisX = new QValueAxis();
    m_axisX->setTitleText("Longitude");
    m_axisX->setLabelFormat("%.6f");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("Latitude");
    m_axisY->setLabelFormat("%.6f");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // Initialize Series
    m_trajectorySeries = new QLineSeries();
    m_trajectorySeries->setName("Trajectory");
    m_chart->addSeries(m_trajectorySeries);
    m_trajectorySeries->attachAxis(m_axisX);
    m_trajectorySeries->attachAxis(m_axisY);

    // Set initial bounds
    m_minLon = 180.0;
    m_maxLon = -180.0;
    m_minLat = 90.0;
    m_maxLat = -90.0;

    // Layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chartView);

    // Update Timer (30 FPS)
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MapView::onTimerTick);
    m_timer->start(33);
}

void MapView::onDataFrameReceived(const DataFrame& frame) {
    double lat = frame.latitude;
    double lon = frame.longitude;

    // Validate GPS data gracefully
    if (lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0) {
        // Exclude exact 0.0, 0.0 as it usually implies lack of lock
        if (lat != 0.0 || lon != 0.0) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_buffer.push_back(QPointF(lon, lat));
        }
    }
}

void MapView::onTimerTick() {
    std::vector<QPointF> localBuffer;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty()) {
            return;
        }
        std::swap(m_buffer, localBuffer);
    }

    bool boundsChanged = false;

    for (const QPointF& pt : localBuffer) {
        m_trajectorySeries->append(pt);

        if (pt.x() < m_minLon) { m_minLon = pt.x(); boundsChanged = true; }
        if (pt.x() > m_maxLon) { m_maxLon = pt.x(); boundsChanged = true; }
        if (pt.y() < m_minLat) { m_minLat = pt.y(); boundsChanged = true; }
        if (pt.y() > m_maxLat) { m_maxLat = pt.y(); boundsChanged = true; }

        m_hasData = true;
    }

    if (boundsChanged && m_hasData) {
        // Prevent collapsing axes if all points are at the exact same coordinate
        double spanX = m_maxLon - m_minLon;
        double spanY = m_maxLat - m_minLat;

        if (spanX < 0.0001) spanX = 0.0001;
        if (spanY < 0.0001) spanY = 0.0001;

        // Add a 10% margin
        double marginX = spanX * 0.1;
        double marginY = spanY * 0.1;

        m_axisX->setRange(m_minLon - marginX, m_maxLon + marginX);
        m_axisY->setRange(m_minLat - marginY, m_maxLat + marginY);
    }
}
