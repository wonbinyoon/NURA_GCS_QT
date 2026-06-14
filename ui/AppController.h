#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <mutex>
#include <queue>
#include "../model/DataFrame.h"

class AppController : public QObject {
    Q_OBJECT

public:
    enum class Mode {
        LIVE,
        REPLAY
    };

    explicit AppController(QObject* parent = nullptr);
    ~AppController() override;

    void receiveDataFrame(const DataFrame& frame);
    void setMode(Mode mode);
    Mode getMode() const;

    // Retrieves the latest frame. Returns true if a frame exists.
    bool getLatestFrame(DataFrame& outFrame) const;

public slots:
    // Serial controls
    void connectSerial(const QString& port, int baudrate);
    void disconnectSerial();

    // Replay controls
    void playReplay();
    void pauseReplay();
    void stopReplay();
    void seekReplay(int timestamp);
    void setReplaySpeed(double speed);

signals:
    void dataUpdated(const DataFrame& frame);
    void logMessage(const QString& message);

    // Serial intents for backend
    void serialConnectRequested(const QString& port, int baudrate);
    void serialDisconnectRequested();

    // Replay intents for backend
    void replayPlayRequested();
    void replayPauseRequested();
    void replayStopRequested();
    void replaySeekRequested(int timestamp);
    void replaySpeedChanged(double speed);

private slots:
    void processFrame();

private:
    Mode m_mode;
    std::queue<DataFrame> m_queue;
    mutable std::mutex m_mutex;
    QTimer* m_timer;

    DataFrame m_latestFrame;
    bool m_hasLatestFrame;
};

#endif // APPCONTROLLER_H
