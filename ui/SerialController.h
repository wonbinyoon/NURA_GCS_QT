#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QString>

class SerialController : public QWidget {
    Q_OBJECT

public:
    explicit SerialController(QWidget* parent = nullptr);
    ~SerialController() override = default;

public slots:
    void setConnectionState(bool connected);

signals:
    void connectRequested(const QString& port, int baudrate);
    void disconnectRequested();

private slots:
    void refreshPorts();
    void onConnectButtonClicked();

private:
    QComboBox* m_portComboBox;
    QComboBox* m_baudRateComboBox;
    QPushButton* m_connectButton;
    QPushButton* m_refreshButton;

    bool m_isConnected;
};

#endif // SERIALCONTROLLER_H
