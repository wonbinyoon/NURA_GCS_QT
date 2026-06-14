#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <QCheckBox>
#include <QTimer>
#include <deque>
#include <mutex>
#include "../model/DataFrame.h"

// Forward declarations for Qt3D
namespace Qt3DExtras {
    class Qt3DWindow;
    class QOrbitCameraController;
    class QConeMesh;
    class QPhongMaterial;
}

namespace Qt3DCore {
    class QEntity;
    class QTransform;
}

namespace Qt3DRender {
    class QCamera;
}

class View3D : public QWidget {
    Q_OBJECT

public:
    explicit View3D(QWidget* parent = nullptr);
    ~View3D() override = default;

public slots:
    void onDataFrameReceived(const DataFrame& frame);

private slots:
    void onTimerTick();

private:
    // UI Controls
    QCheckBox* m_followModeCheckbox;

    // Concurrency
    std::deque<DataFrame> m_buffer;
    std::mutex m_mutex;
    QTimer* m_timer;

    // Qt3D Components
    Qt3DExtras::Qt3DWindow* m_view;
    QWidget* m_container;

    Qt3DCore::QEntity* m_rootEntity;
    Qt3DCore::QEntity* m_rocketEntity;
    Qt3DCore::QTransform* m_rocketTransform;
    Qt3DRender::QCamera* m_camera;
    Qt3DExtras::QOrbitCameraController* m_camController;

    Qt3DExtras::QConeMesh* m_rocketMesh;
    Qt3DExtras::QPhongMaterial* m_rocketMaterial;

    // State tracking
    int m_pointsDrawn;

    // Helper
    void setupScene();
    void spawnTrajectoryPoint(const QVector3D& position);
};

#endif // VIEW3D_H
