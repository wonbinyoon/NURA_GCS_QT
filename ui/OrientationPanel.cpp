#include "OrientationPanel.h"
#include <QVBoxLayout>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

OrientationPanel::OrientationPanel(QWidget* parent)
    : QWidget(parent) {

    // UI Setup
    m_indicatorCheckbox = new QCheckBox("Show 3D Indicator", this);
    m_indicatorCheckbox->setChecked(true);

    m_view = new Qt3DExtras::Qt3DWindow();
    m_view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x222222)));
    m_container = QWidget::createWindowContainer(m_view, this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_indicatorCheckbox);
    layout->addWidget(m_container, 1);

    // Initialize initial quaternion to identity
    m_latestQuat = QQuaternion();

    // Scene Setup
    m_rootEntity = new Qt3DCore::QEntity();
    setupScene();
    m_view->setRootEntity(m_rootEntity);

    connect(m_indicatorCheckbox, &QCheckBox::toggled, this, &OrientationPanel::onIndicatorToggled);

    // Update Timer (30 FPS)
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OrientationPanel::onTimerTick);
    m_timer->start(33);
}

void OrientationPanel::setupScene() {
    // Camera
    Qt3DRender::QCamera* camera = m_view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.0f, 0.0f, 15.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));

    // Lighting
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1.0f);
    Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(0.0f, 0.0f, 20.0f));
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);

    // Navball Setup
    m_navballEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_navballTransform = new Qt3DCore::QTransform();
    m_navballTransform->setTranslation(QVector3D(-3.0f, 0.0f, 0.0f)); // Position Navball on left

    Qt3DExtras::QSphereMesh* sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRadius(2.0f);
    sphereMesh->setRings(30);
    sphereMesh->setSlices(30);

    Qt3DExtras::QPhongMaterial* sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(QColor(QRgb(0x4466cc)));

    m_navballEntity->addComponent(sphereMesh);
    m_navballEntity->addComponent(sphereMaterial);
    m_navballEntity->addComponent(m_navballTransform);

    // Navball Axis Indicator (Cylinder through poles)
    Qt3DCore::QEntity* navballAxis = new Qt3DCore::QEntity(m_navballEntity);
    Qt3DExtras::QCylinderMesh* axisMesh = new Qt3DExtras::QCylinderMesh();
    axisMesh->setRadius(0.1f);
    axisMesh->setLength(4.5f);
    Qt3DExtras::QPhongMaterial* axisMaterial = new Qt3DExtras::QPhongMaterial();
    axisMaterial->setDiffuse(QColor(QRgb(0xff0000))); // Red axis
    Qt3DCore::QTransform* axisTransform = new Qt3DCore::QTransform();
    // Cylinder by default goes along Y. Let's make it go along Z for the main pole.
    axisTransform->setRotationX(90.0f);
    navballAxis->addComponent(axisMesh);
    navballAxis->addComponent(axisMaterial);
    navballAxis->addComponent(axisTransform);

    // 3D Indicator Setup (Rocket/Cone)
    m_indicatorEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_indicatorTransform = new Qt3DCore::QTransform();
    m_indicatorTransform->setTranslation(QVector3D(3.0f, 0.0f, 0.0f)); // Position Indicator on right

    Qt3DExtras::QConeMesh* coneMesh = new Qt3DExtras::QConeMesh();
    coneMesh->setTopRadius(0.0f);
    coneMesh->setBottomRadius(1.0f);
    coneMesh->setLength(4.0f);
    coneMesh->setRings(20);
    coneMesh->setSlices(20);

    Qt3DExtras::QPhongMaterial* coneMaterial = new Qt3DExtras::QPhongMaterial();
    coneMaterial->setDiffuse(QColor(QRgb(0xffa500))); // Orange cone

    // Cone points up (Y) by default. Rotate to point forward (Z) to match typical flight dynamics
    Qt3DCore::QTransform* conePreTransform = new Qt3DCore::QTransform();
    conePreTransform->setRotationX(90.0f);

    Qt3DCore::QEntity* coneVisual = new Qt3DCore::QEntity(m_indicatorEntity);
    coneVisual->addComponent(coneMesh);
    coneVisual->addComponent(coneMaterial);
    coneVisual->addComponent(conePreTransform);

    m_indicatorEntity->addComponent(m_indicatorTransform);
}

void OrientationPanel::onDataFrameReceived(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    // DataFrame quat is [w, x, y, z] or [x, y, z, w]. Assuming QQuaternion(scalar, x, y, z)
    // Standard notation for quat[4] array is usually [w, x, y, z] or [x, y, z, w].
    // Let's assume standard math notation: frame.quat[0] is w.
    m_latestQuat = QQuaternion(frame.quat[0], frame.quat[1], frame.quat[2], frame.quat[3]);
    m_latestQuat.normalize();
}

void OrientationPanel::onTimerTick() {
    QQuaternion quat;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        quat = m_latestQuat;
    }

    m_navballTransform->setRotation(quat);
    m_indicatorTransform->setRotation(quat);
}

void OrientationPanel::onIndicatorToggled(bool checked) {
    m_indicatorEntity->setEnabled(checked);
}
