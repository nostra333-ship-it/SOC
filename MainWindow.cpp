#include "MainWindow.h"
#include "CameraGrid.h"
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setupMenuBar();
    setupToolbar();

    setWindowTitle("CCTV Simulator");
    resize(1280, 720);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    m_cameraGrid = new CameraGrid(this);
    setCentralWidget(m_cameraGrid);
    m_cameraGrid->setGridSize(4);

    connect(m_cameraGrid, &CameraGrid::cameraSelected,
        this, &MainWindow::onCameraSelected);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* injectAction = fileMenu->addAction("&Inject Video...");
    connect(injectAction, &QAction::triggered, this, &MainWindow::onInjectVideo);
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("&Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QMenu* viewMenu = menuBar->addMenu("&View");
    QAction* grid4Action = viewMenu->addAction("4 Cameras");
    connect(grid4Action, &QAction::triggered, [this]() { m_cameraGrid->setGridSize(4); });
    QAction* grid9Action = viewMenu->addAction("9 Cameras");
    connect(grid9Action, &QAction::triggered, [this]() { m_cameraGrid->setGridSize(9); });
    QAction* grid16Action = viewMenu->addAction("16 Cameras");
    connect(grid16Action, &QAction::triggered, [this]() { m_cameraGrid->setGridSize(16); });

    setMenuBar(menuBar);
}

void MainWindow::setupToolbar()
{
    QToolBar* toolbar = addToolBar("Controls");
    QAction* injectAction = toolbar->addAction("🎬 Inject Video");
    connect(injectAction, &QAction::triggered, this, &MainWindow::onInjectVideo);
    toolbar->addSeparator();
    toolbar->addAction("🖱️ Left=PTZ | Right=Play/Pause");
}

void MainWindow::onInjectVideo()
{
    m_selectedVideo = QFileDialog::getOpenFileName(
        this, "Select Video", "", "Video Files (*.mp4 *.avi *.mov)");

    if (!m_selectedVideo.isEmpty()) {
        statusBar()->showMessage("Video ready - click on a camera to inject", 3000);
    }
}

void MainWindow::onCameraSelected(int index)
{
    if (!m_selectedVideo.isEmpty()) {
        auto cam = m_cameraGrid->cameraAt(index);
        if (cam) {
            cam->loadVideo(m_selectedVideo);
            statusBar()->showMessage(QString("Video injected into Camera %1").arg(index + 1), 2000);
            m_selectedVideo.clear();
        }
    }
}