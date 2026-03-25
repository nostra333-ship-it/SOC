#ifndef CAMERAGRID_H
#define CAMERAGRID_H

#include <QWidget>
#include <QGridLayout>
#include <vector>

class CameraWidget;  // Forward declaration

class CameraGrid : public QWidget
{
    Q_OBJECT

public:
    explicit CameraGrid(QWidget* parent = nullptr);
    ~CameraGrid();

    void setGridSize(int cameras);
    CameraWidget* cameraAt(int index);

signals:
    void cameraSelected(int index);

private:
    void createGrid();
    void clearGrid();

    std::vector<CameraWidget*> m_cameras;
    QGridLayout* m_layout;
    int m_gridSize;
};

#endif