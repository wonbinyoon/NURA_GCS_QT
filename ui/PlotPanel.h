#ifndef PLOTPANEL_H
#define PLOTPANEL_H

#include <QWidget>
#include <QCheckBox>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <deque>
#include <mutex>
#include "../model/DataFrame.h"

class PlotPanel : public QWidget {
    Q_OBJECT

public:
    explicit PlotPanel(QWidget* parent = nullptr);
    ~PlotPanel() override = default;

public slots:
    void onDataFrameReceived(const DataFrame& frame);

private slots:
    void onTimerTick();

private:
    QChart* m_chart;
    QChartView* m_chartView;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;

    QLineSeries* m_seriesAltitude;
    QLineSeries* m_seriesVelocityZ;
    QLineSeries* m_seriesAccelZ;

    QCheckBox* m_checkAltitude;
    QCheckBox* m_checkVelocityZ;
    QCheckBox* m_checkAccelZ;

    QTimer* m_timer;

    std::deque<DataFrame> m_buffer;
    std::mutex m_mutex;
};

#endif // PLOTPANEL_H
