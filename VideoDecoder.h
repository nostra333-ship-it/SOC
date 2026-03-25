#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <QMutex>
#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoDecoder : public QObject
{
    Q_OBJECT

public:
    explicit VideoDecoder(QObject* parent = nullptr);
    ~VideoDecoder();

    bool load(const QString& filePath);
    void play();
    void pause();
    void stop();
    void setLoop(bool enabled);      // NEW: Enable/disable loop
    void setFreeze(bool frozen);     // NEW: Freeze current frame
    bool isPlaying() const;
    bool isFrozen() const { return m_frozen; }  // NEW: Check if frozen

signals:
    void frameReady(const QImage& frame);
    void error(const QString& message);
    void playbackEnded();            // NEW: Signal when video ends

private:
    void decodeLoop();
    QImage convertFrameToImage(AVFrame* frame);

    std::atomic<bool> m_running;
    std::atomic<bool> m_playing;
    std::atomic<bool> m_loop;        // NEW: Loop flag
    std::atomic<bool> m_frozen;      // NEW: Freeze flag
    QThread* m_decodeThread;

    AVFormatContext* m_formatCtx;
    AVCodecContext* m_codecCtx;
    SwsContext* m_swsCtx;
    int m_videoStreamIndex;
    AVPacket* m_packet;
    AVFrame* m_frame;
    QMutex m_mutex;
    QImage m_lastFrame;              // NEW: Store last frame for freeze
};

#endif