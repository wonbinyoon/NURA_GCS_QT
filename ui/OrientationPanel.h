#ifndef ORIENTATIONPANEL_H
#define ORIENTATIONPANEL_H

#include <QWidget>
#include <QCheckBox>
#include <QTimer>
#include <QQuaternion>
#include <mutex>
#include "../model/DataFrame.h"

namespace Qt3DExtras {
    class Qt3DWindow;
}

namespace Qt3DCore {
    class QEntity;
    class QTransform;
}

class OrientationPanel : public QWidget {
    Q_OBJECT

public:
    explicit OrientationPanel(QWidget* parent = nullptr);
    ~OrientationPanel() override = default;

public slots:
    void onDataFrameReceived(const DataFrame& frame);

private slots:
    void onTimerTick();
    void onIndicatorToggled(bool checked);

private:
    QCheckBox* m_indicatorCheckbox;

    std::mutex m_mutex;
    QQuaternion m_latestQuat;
    QTimer* m_timer;

    Qt3DExtras::Qt3DWindow* m_view;
    QWidget* m_container;

    Qt3DCore::QEntity* m_rootEntity;

    // Navball
    Qt3DCore::QEntity* m_navballEntity;
    Qt3DCore::QTransform* m_navballTransform;

    // 3D Indicator
    Qt3DCore::QEntity* m_indicatorEntity;
    Qt3DCore::QTransform* m_indicatorTransform;

    void setupScene();
};

#endif // ORIENTATIONPANEL_H
