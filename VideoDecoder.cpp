#include "VideoDecoder.h"
#include <QDebug>
#include <QElapsedTimer>

VideoDecoder::VideoDecoder(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_playing(false)
    , m_loop(true)           // NEW: Default to loop enabled
    , m_frozen(false)        // NEW: Default not frozen
    , m_decodeThread(nullptr)
    , m_formatCtx(nullptr)
    , m_codecCtx(nullptr)
    , m_swsCtx(nullptr)
    , m_videoStreamIndex(-1)
    , m_packet(nullptr)
    , m_frame(nullptr)
{
    avformat_network_init();
}

VideoDecoder::~VideoDecoder()
{
    stop();
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
    }
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    avformat_network_deinit();
}

bool VideoDecoder::load(const QString& filePath)
{
    QMutexLocker locker(&m_mutex);

    // Close existing
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = nullptr;
    }

    // Open file
    if (avformat_open_input(&m_formatCtx, filePath.toUtf8().constData(), nullptr, nullptr) != 0) {
        emit error("Failed to open file");
        return false;
    }

    // Find stream info
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        emit error("Failed to find stream info");
        return false;
    }

    // Find video stream
    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        emit error("No video stream found");
        return false;
    }

    // Get codec
    AVCodecParameters* codecPar = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecPar->codec_id);
    if (!codec) {
        emit error("Codec not found");
        return false;
    }

    // Create codec context
    m_codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecCtx, codecPar);

    // Open codec
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        emit error("Failed to open codec");
        return false;
    }

    // Create scaling context
    m_swsCtx = sws_getContext(
        m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
        m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    // Allocate packet and frame
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();

    return true;
}

void VideoDecoder::play()
{
    m_playing = true;
    m_frozen = false;  // Unfreeze when playing

    if (!m_running) {
        m_running = true;
        m_decodeThread = QThread::create([this]() { decodeLoop(); });
        m_decodeThread->start();
    }
}

void VideoDecoder::pause()
{
    m_playing = false;
}

void VideoDecoder::stop()
{
    m_running = false;
    m_playing = false;

    if (m_decodeThread) {
        m_decodeThread->quit();
        m_decodeThread->wait();
        delete m_decodeThread;
        m_decodeThread = nullptr;
    }
}

void VideoDecoder::setLoop(bool enabled)
{
    m_loop = enabled;
}

void VideoDecoder::setFreeze(bool frozen)
{
    m_frozen = frozen;
}

bool VideoDecoder::isPlaying() const
{
    return m_playing && !m_frozen;
}

void VideoDecoder::decodeLoop()
{
    QElapsedTimer frameTimer;
    frameTimer.start();

    while (m_running && m_formatCtx) {
        // If frozen, just keep sending the last frame
        if (m_frozen) {
            QThread::msleep(33); // ~30 fps
            continue;
        }

        if (!m_playing) {
            QThread::msleep(10);
            continue;
        }

        // Read packet
        int ret = av_read_frame(m_formatCtx, m_packet);
        if (ret >= 0) {
            if (m_packet->stream_index == m_videoStreamIndex) {
                // Send packet to decoder
                if (avcodec_send_packet(m_codecCtx, m_packet) == 0) {
                    // Receive decoded frames
                    while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
                        QImage image = convertFrameToImage(m_frame);
                        if (!image.isNull()) {
                            m_lastFrame = image;  // Store for freeze
                            emit frameReady(image);
                        }
                    }
                }
            }
            av_packet_unref(m_packet);
        }
        else {
            // End of file - loop or stop
            if (m_loop) {
                // Seek back to beginning
                av_seek_frame(m_formatCtx, m_videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
                avcodec_flush_buffers(m_codecCtx);
                emit playbackEnded();
            }
            else {
                // Stop playing
                m_playing = false;
                emit playbackEnded();
                break;
            }
        }

        // Maintain ~30 FPS playback speed
        int elapsed = frameTimer.elapsed();
        int targetDelay = 33; // ~30 fps
        if (elapsed < targetDelay) {
            QThread::msleep(targetDelay - elapsed);
        }
        frameTimer.restart();
    }
}

QImage VideoDecoder::convertFrameToImage(AVFrame* frame)
{
    if (!frame || !m_swsCtx) return QImage();

    // Create buffer for RGBA image
    uint8_t* buffer = new uint8_t[frame->width * frame->height * 4];
    uint8_t* dstData[] = { buffer };
    int dstLinesize[] = { frame->width * 4 };

    // Convert frame to RGBA
    sws_scale(m_swsCtx, frame->data, frame->linesize, 0, frame->height,
        dstData, dstLinesize);

    // Create QImage and copy data
    QImage image(buffer, frame->width, frame->height, QImage::Format_RGBA8888);
    QImage copy = image.copy();
    delete[] buffer;

    return copy;
}