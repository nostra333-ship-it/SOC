#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CameraGrid;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onInjectVideo();
    void onCameraSelected(int index);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolbar();

    CameraGrid* m_cameraGrid;
    QString m_selectedVideo;
};

#endif