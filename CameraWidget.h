#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMutex>
#include <QImage>
#include <QPoint>
#include <memory>

class VideoDecoder;

class CameraWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit CameraWidget(int cameraId, QWidget* parent = nullptr);
    ~CameraWidget();

    void loadVideo(const QString& filePath);
    void setSelected(bool selected);

signals:
    void clicked();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onFrameReady(const QImage& frame);

private:
    void updateTexture(const QImage& image);
    void renderTexture();
    void renderOverlay();

    int m_cameraId;
    bool m_selected;

    // PTZ
    float m_pan = 0.0f;
    float m_tilt = 0.0f;
    float m_zoom = 1.0f;
    QPoint m_lastMouse;
    bool m_dragging = false;

    // Video
    std::unique_ptr<VideoDecoder> m_decoder;
    QImage m_currentFrame;
    QMutex m_frameMutex;
    bool m_hasFrame = false;

    // OpenGL
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    GLuint m_textureId = 0;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_model;
};

#endif