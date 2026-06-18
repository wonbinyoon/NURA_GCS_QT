#include "View3D.h"
#include <QVBoxLayout>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <QQuaternion>

View3D::View3D(QWidget* parent)
    : QWidget(parent), m_pointsDrawn(0) {

    m_followModeCheckbox = new QCheckBox("Follow Mode", this);
    m_followModeCheckbox->setChecked(true);

    // Initialize 3D Window
    m_view = new Qt3DExtras::Qt3DWindow();
    m_view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    m_container = QWidget::createWindowContainer(m_view, this);

    // Layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_followModeCheckbox);
    layout->addWidget(m_container, 1); // 1 = stretch factor

    // Setup Scene
    m_rootEntity = new Qt3DCore::QEntity();
    setupScene();
    m_view->setRootEntity(m_rootEntity);

    // Update Timer (30 FPS)
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &View3D::onTimerTick);
    m_timer->start(33);
}

void View3D::setupScene() {
    // Camera
    m_camera = m_view->camera();
    m_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_camera->setPosition(QVector3D(0.0f, 50.0f, 100.0f));
    m_camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));

    // Camera controls
    m_camController = new Qt3DExtras::QOrbitCameraController(m_rootEntity);
    m_camController->setLinearSpeed(50.0f);
    m_camController->setLookSpeed(180.0f);
    m_camController->setCamera(m_camera);

    // Light
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1.0f);
    Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(0.0f, 100.0f, 0.0f));
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);

    // Rocket Entity
    m_rocketEntity = new Qt3DCore::QEntity(m_rootEntity);

    m_rocketMesh = new Qt3DExtras::QConeMesh();
    m_rocketMesh->setTopRadius(0.0f);
    m_rocketMesh->setBottomRadius(1.0f);
    m_rocketMesh->setLength(5.0f);
    m_rocketMesh->setRings(20);
    m_rocketMesh->setSlices(20);

    m_rocketMaterial = new Qt3DExtras::QPhongMaterial();
    m_rocketMaterial->setDiffuse(QColor(QRgb(0xff0000)));

    m_rocketTransform = new Qt3DCore::QTransform();
    m_rocketTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

    // Rotate cone to align with Z axis forward
    Qt3DCore::QTransform* preTransform = new Qt3DCore::QTransform();
    preTransform->setRotationX(90.0f);
    Qt3DCore::QEntity* visualEntity = new Qt3DCore::QEntity(m_rocketEntity);
    visualEntity->addComponent(m_rocketMesh);
    visualEntity->addComponent(m_rocketMaterial);
    visualEntity->addComponent(preTransform);

    m_rocketEntity->addComponent(m_rocketTransform);
}

void View3D::onDataFrameReceived(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffer.push_back(frame);
}

void View3D::onTimerTick() {
    std::deque<DataFrame> localBuffer;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::swap(m_buffer, localBuffer);
    }

    if (localBuffer.empty()) return;

    QVector3D latestPos;
    QQuaternion latestQuat;

    for (const auto& frame : localBuffer) {
        float x = frame.position_ned[0]; // North
        float z = frame.position_ned[1]; // East
        float y = -frame.position_ned[2]; // Down -> -Y (so Y is Up)

        QVector3D pos(x, y, z);
        latestPos = pos;

        // Quat [w, x, y, z] mapped. For NED to Qt3D mapping:
        // We assume frame.quat[0] = w.
        QQuaternion q(frame.quat[0], frame.quat[1], frame.quat[2], frame.quat[3]);
        q.normalize();
        latestQuat = q;

        // Spawn a trajectory point periodically (e.g. every 10 frames)
        if (m_pointsDrawn % 10 == 0) {
            spawnTrajectoryPoint(pos);
        }
        m_pointsDrawn++;
    }

    // Update rocket transform
    m_rocketTransform->setTranslation(latestPos);
    m_rocketTransform->setRotation(latestQuat);

    // Update camera if follow mode is active
    if (m_followModeCheckbox->isChecked()) {
        m_camera->setViewCenter(latestPos);
    }
}

void View3D::spawnTrajectoryPoint(const QVector3D& position) {
    Qt3DCore::QEntity* pointEntity = new Qt3DCore::QEntity(m_rootEntity);

    Qt3DExtras::QSphereMesh* sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRadius(0.5f);

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0x00ff00))); // Green trajectory

    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform();
    transform->setTranslation(position);

    pointEntity->addComponent(sphereMesh);
    pointEntity->addComponent(material);
    pointEntity->addComponent(transform);
}
