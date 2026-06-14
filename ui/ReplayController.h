#ifndef REPLAYCONTROLLER_H
#define REPLAYCONTROLLER_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSlider>

class ReplayController : public QWidget {
    Q_OBJECT

public:
    explicit ReplayController(QWidget* parent = nullptr);
    ~ReplayController() override = default;

public slots:
    void setSliderRange(int min, int max);
    void setSliderPosition(int position);

signals:
    void playRequested();
    void pauseRequested();
    void stopRequested();
    void seekRequested(int timestamp);
    void speedChanged(double speed);

private slots:
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onSliderMoved(int value);
    void onSpeedValueChanged(double value);

private:
    QPushButton* m_playButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QDoubleSpinBox* m_speedSpinBox;
    QSlider* m_seekSlider;
};

#endif // REPLAYCONTROLLER_H
