#ifndef CONSOLEPANEL_H
#define CONSOLEPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTimer>
#include <QString>
#include <vector>
#include <mutex>

class ConsolePanel : public QWidget {
    Q_OBJECT

public:
    explicit ConsolePanel(QWidget* parent = nullptr);
    ~ConsolePanel() override = default;

public slots:
    void logMessage(const QString& message);

private slots:
    void onTimerTick();

private:
    QPlainTextEdit* m_textEdit;
    QTimer* m_timer;

    std::mutex m_mutex;
    std::vector<QString> m_buffer;
};

#endif // CONSOLEPANEL_H
