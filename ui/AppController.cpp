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
