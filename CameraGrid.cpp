#include "CameraGrid.h"
#include "CameraWidget.h"  // Add this include

CameraGrid::CameraGrid(QWidget* parent)
    : QWidget(parent)
    , m_layout(new QGridLayout(this))
    , m_gridSize(4)
{
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(2, 2, 2, 2);
    createGrid();
}

CameraGrid::~CameraGrid()
{
    clearGrid();
}

void CameraGrid::setGridSize(int cameras)
{
    if (cameras == m_gridSize) return;
    m_gridSize = cameras;
    createGrid();
}

void CameraGrid::clearGrid()
{
    for (auto cam : m_cameras) {
        if (cam) {
            m_layout->removeWidget(cam);
            delete cam;
        }
    }
    m_cameras.clear();
}

void CameraGrid::createGrid()
{
    clearGrid();

    int cols = 2;
    if (m_gridSize == 9) cols = 3;
    if (m_gridSize == 16) cols = 4;

    for (int i = 0; i < m_gridSize; ++i) {
        CameraWidget* cam = new CameraWidget(i + 1, this);

        // Connect the clicked signal
        connect(cam, &CameraWidget::clicked, [this, i]() {
            emit cameraSelected(i);
            });

        int row = i / cols;
        int col = i % cols;
        m_layout->addWidget(cam, row, col);
        m_cameras.push_back(cam);
    }
}

CameraWidget* CameraGrid::cameraAt(int index)
{
    if (index >= 0 && index < (int)m_cameras.size()) {
        return m_cameras[index];
    }
    return nullptr;
}