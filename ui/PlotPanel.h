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
    struct PlotGroup {
        QChart* chart;
        QChartView* view;
        QValueAxis* axisX;
        QValueAxis* axisY;
        QList<QLineSeries*> series;
        QList<QCheckBox*> checkboxes;
    };

    PlotGroup m_posAltGroup;
    PlotGroup m_velGroup;
    PlotGroup m_accGroup;
    PlotGroup m_imuGroup;
    PlotGroup m_envGroup;

    QTimer* m_timer;

    std::deque<DataFrame> m_buffer;
    std::mutex m_mutex;

    void setupGroup(PlotGroup& group, const QString& title, const QStringList& seriesNames);
    void updateGroup(PlotGroup& group, const QList<QList<QPointF>>& data, double minX, double maxX, double minY, double maxY);
};

#endif // PLOTPANEL_H
