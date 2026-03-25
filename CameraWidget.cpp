#include "CameraWidget.h"
#include "VideoDecoder.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QMutexLocker>
#include <QDebug>

CameraWidget::CameraWidget(int cameraId, QWidget* parent)
    : QOpenGLWidget(parent)
    , m_cameraId(cameraId)
    , m_selected(false)
    , m_decoder(std::make_unique<VideoDecoder>(this))
    , m_hasFrame(false)
{
    setMinimumSize(200, 150);
    connect(m_decoder.get(), &VideoDecoder::frameReady, this, &CameraWidget::onFrameReady);
}

CameraWidget::~CameraWidget()
{
    makeCurrent();
    if (m_textureId) glDeleteTextures(1, &m_textureId);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    doneCurrent();
}

void CameraWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Simple shader
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "in vec2 aPos; in vec2 aTex; out vec2 vTex; uniform mat4 m; void main() { gl_Position = m * vec4(aPos,0,1); vTex = aTex; }");
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "in vec2 vTex; out vec4 color; uniform sampler2D tex; void main() { color = texture(tex, vTex); }");
    m_program->link();

    float vert[] = { -1,1,0,0, 1,1,1,0, 1,-1,1,1, -1,-1,0,1 };
    unsigned int idx[] = { 0,1,2, 2,3,0 };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CameraWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.ortho(-1, 1, -1, 1, -1, 1);
}

void CameraWidget::paintGL()
{
    {
        QMutexLocker lock(&m_frameMutex);
        if (m_hasFrame && !m_currentFrame.isNull()) {
            QImage img = m_currentFrame.convertToFormat(QImage::Format_RGBA8888);
            glBindTexture(GL_TEXTURE_2D, m_textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
            m_hasFrame = false;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    m_model.setToIdentity();
    m_model.translate(m_pan, -m_tilt, 0);
    m_model.scale(m_zoom, m_zoom, 1);

    m_program->bind();
    m_program->setUniformValue("m", m_proj * m_model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_program->release();

    renderOverlay();
}

void CameraWidget::renderOverlay()
{
    QPainter p(this);
    p.fillRect(10, 10, 60, 25, QColor(0, 0, 0, 180));
    p.setPen(Qt::white);
    p.drawText(10, 28, QString("CAM %1").arg(m_cameraId));

    if (m_selected) {
        p.setPen(QPen(Qt::green, 3));
        p.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}

void CameraWidget::loadVideo(const QString& path)
{
    if (m_decoder->load(path)) {
        m_decoder->play();
        qDebug() << "Camera" << m_cameraId << "playing:" << path;
    }
}

void CameraWidget::onFrameReady(const QImage& frame)
{
    QMutexLocker lock(&m_frameMutex);
    m_currentFrame = frame;
    m_hasFrame = true;
    update();
}

void CameraWidget::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void CameraWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        emit clicked();
        m_dragging = true;
        m_lastMouse = e->pos();
        setCursor(Qt::ClosedHandCursor);
        e->accept();
    }
    QOpenGLWidget::mousePressEvent(e);
}

void CameraWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_dragging) {
        QPoint d = e->pos() - m_lastMouse;
        m_pan += d.x() * 0.003f;
        m_tilt += d.y() * 0.003f;
        m_pan = qBound(-0.8f, m_pan, 0.8f);
        m_tilt = qBound(-0.8f, m_tilt, 0.8f);
        m_lastMouse = e->pos();
        update();
        e->accept();
    }
    QOpenGLWidget::mouseMoveEvent(e);
}

void CameraWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
        e->accept();
    }
    QOpenGLWidget::mouseReleaseEvent(e);
}

void CameraWidget::wheelEvent(QWheelEvent* e)
{
    float delta = e->angleDelta().y();
    m_zoom += delta * 0.002f;
    m_zoom = qBound(1.0f, m_zoom, 4.0f);
    update();
    e->accept();
}