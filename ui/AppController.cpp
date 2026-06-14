#include "AppController.h"

AppController::AppController(QObject* parent)
    : QObject(parent), m_mode(Mode::LIVE) {
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

    // Bound the queue to prevent infinite growth. 10 frames is plenty.
    while (m_queue.size() > 10) {
        m_queue.pop();
    }
}

void AppController::setMode(Mode mode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_mode != mode) {
        m_mode = mode;
        // Clear the buffer when switching modes
        std::queue<DataFrame> empty;
        std::swap(m_queue, empty);
    }
}

AppController::Mode AppController::getMode() const {
    return m_mode; // No lock needed for simple read, but could lock if needed
}

void AppController::processFrame() {
    DataFrame latestFrame;
    bool hasData = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_queue.empty()) {
            hasData = true;
            // Pop all frames and keep the latest one
            while (!m_queue.empty()) {
                latestFrame = m_queue.front();
                m_queue.pop();
            }
        }
    }

    if (hasData) {
        emit dataUpdated(latestFrame);
    }
}
