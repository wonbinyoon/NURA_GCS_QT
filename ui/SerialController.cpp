#include "SerialController.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSerialPortInfo>

SerialController::SerialController(QWidget* parent)
    : QWidget(parent), m_isConnected(false) {

    // Initialize UI components
    m_portComboBox = new QComboBox(this);
    m_baudRateComboBox = new QComboBox(this);
    m_connectButton = new QPushButton("Connect", this);
    m_refreshButton = new QPushButton("Refresh", this);

    // Populate common baud rates
    m_baudRateComboBox->addItem("9600", 9600);
    m_baudRateComboBox->addItem("19200", 19200);
    m_baudRateComboBox->addItem("38400", 38400);
    m_baudRateComboBox->addItem("57600", 57600);
    m_baudRateComboBox->addItem("115200", 115200);
    m_baudRateComboBox->setCurrentText("115200"); // Default

    // Set up layouts
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("COM Port:", m_portComboBox);
    formLayout->addRow("Baud Rate:", m_baudRateComboBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addWidget(m_connectButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    // Connect internal signals and slots
    connect(m_refreshButton, &QPushButton::clicked, this, &SerialController::refreshPorts);
    connect(m_connectButton, &QPushButton::clicked, this, &SerialController::onConnectButtonClicked);

    // Initial port scan
    refreshPorts();
    setConnectionState(false);
}

void SerialController::refreshPorts() {
    m_portComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& portInfo : ports) {
        m_portComboBox->addItem(portInfo.portName());
    }
}

void SerialController::setConnectionState(bool connected) {
    m_isConnected = connected;

    // Update UI based on state
    m_portComboBox->setDisabled(connected);
    m_baudRateComboBox->setDisabled(connected);
    m_refreshButton->setDisabled(connected);

    if (connected) {
        m_connectButton->setText("Disconnect");
    } else {
        m_connectButton->setText("Connect");
    }
}

void SerialController::onConnectButtonClicked() {
    if (m_isConnected) {
        emit disconnectRequested();
    } else {
        QString port = m_portComboBox->currentText();
        int baudrate = m_baudRateComboBox->currentData().toInt();
        if (!port.isEmpty()) {
            emit connectRequested(port, baudrate);
        }
    }
}
