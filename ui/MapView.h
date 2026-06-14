#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <vector>
#include <mutex>
#include <QPointF>
#include "../model/DataFrame.h"

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);
    ~MapView() override = default;

public slots:
    void onDataFrameReceived(const DataFrame& frame);

private slots:
    void onTimerTick();

private:
    QChart* m_chart;
    QChartView* m_chartView;
    QValueAxis* m_axisX; // Longitude
    QValueAxis* m_axisY; // Latitude
    QLineSeries* m_trajectorySeries;

    QTimer* m_timer;

    std::mutex m_mutex;
    std::vector<QPointF> m_buffer;

    // Bounds tracking
    double m_minLon;
    double m_maxLon;
    double m_minLat;
    double m_maxLat;
    bool m_hasData;
};

#endif // MAPVIEW_H
