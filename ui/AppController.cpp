#include "AppController.h"

AppController::AppController(QObject* parent)
    : QObject(parent), m_mode(Mode::LIVE), m_hasLatestFrame(false) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &AppController::processFrame);
    // 33 ms is approximately 30 FPS
    m_timer->start(33);
}

AppController::~AppController() {
    m_timer->stop();
}

void AppController::receiveDataFrame(const DataFrame& frame) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(frame);
}

void AppController::setMode(Mode mode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_mode != mode) {
        m_mode = mode;
        // Clear the buffer when switching modes
        std::queue<DataFrame> empty;
        std::swap(m_queue, empty);
        m_hasLatestFrame = false;

        emit logMessage(QString("Switched mode to %1").arg(mode == Mode::LIVE ? "LIVE" : "REPLAY"));
    }
}

AppController::Mode AppController::getMode() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mode;
}

bool AppController::getLatestFrame(DataFrame& outFrame) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_hasLatestFrame) {
        outFrame = m_latestFrame;
        return true;
    }
    return false;
}

void AppController::processFrame() {
    std::queue<DataFrame> localQueue;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::swap(m_queue, localQueue);

        if (!localQueue.empty()) {
            m_latestFrame = localQueue.back();
            m_hasLatestFrame = true;
        }
    }

    while (!localQueue.empty()) {
        emit dataUpdated(localQueue.front());
        localQueue.pop();
    }
}

void AppController::connectSerial(const QString& port, int baudrate) {
    emit logMessage(QString("Requesting serial connection on %1 at %2 bps").arg(port).arg(baudrate));
    emit serialConnectRequested(port, baudrate);
}

void AppController::disconnectSerial() {
    emit logMessage("Requesting serial disconnect");
    emit serialDisconnectRequested();
}

void AppController::playReplay() {
    emit logMessage("Requesting replay play");
    emit replayPlayRequested();
}

void AppController::pauseReplay() {
    emit logMessage("Requesting replay pause");
    emit replayPauseRequested();
}

void AppController::stopReplay() {
    emit logMessage("Requesting replay stop");
    emit replayStopRequested();
}

void AppController::seekReplay(int timestamp) {
    emit logMessage(QString("Requesting replay seek to timestamp %1").arg(timestamp));
    emit replaySeekRequested(timestamp);
}

void AppController::setReplaySpeed(double speed) {
    emit logMessage(QString("Requesting replay speed change to %1x").arg(speed));
    emit replaySpeedChanged(speed);
}
