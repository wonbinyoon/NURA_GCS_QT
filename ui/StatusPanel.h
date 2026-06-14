#ifndef STATUSPANEL_H
#define STATUSPANEL_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <mutex>
#include "../model/DataFrame.h"

class StatusPanel : public QWidget {
    Q_OBJECT

public:
    explicit StatusPanel(QWidget* parent = nullptr);
    ~StatusPanel() override = default;

public slots:
    void onDataFrameReceived(const DataFrame& frame);

private slots:
    void onTimerTick();

private:
    QLabel* m_fsmLabel;
    QLabel* m_altLabel;
    QLabel* m_velLabel;
    QLabel* m_accLabel;
    QLabel* m_gpsLabel;

    QTimer* m_timer;

    std::mutex m_mutex;
    DataFrame m_latestFrame;
    bool m_hasData;
};

#endif // STATUSPANEL_H
