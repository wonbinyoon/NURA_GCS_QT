#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QTimer>
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

signals:
    void dataUpdated(const DataFrame& frame);

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
