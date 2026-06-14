#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QSplitter>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    QWidget* m_controlPanel;
    QWidget* m_visualizationArea;
    QWidget* m_console;
};

#endif // MAINWINDOW_H
