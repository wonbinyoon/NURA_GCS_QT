#include "ConsolePanel.h"
#include <QVBoxLayout>

ConsolePanel::ConsolePanel(QWidget* parent)
    : QWidget(parent) {

    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    // Ensure reasonable maximum block count to prevent infinite memory usage
    m_textEdit->setMaximumBlockCount(1000);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_textEdit);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ConsolePanel::onTimerTick);
    m_timer->start(33);
}

void ConsolePanel::logMessage(const QString& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffer.push_back(message);
}

void ConsolePanel::onTimerTick() {
    std::vector<QString> localBuffer;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty()) {
            return;
        }
        std::swap(m_buffer, localBuffer);
    }

    for (const QString& msg : localBuffer) {
        m_textEdit->appendPlainText(msg);
    }
}
