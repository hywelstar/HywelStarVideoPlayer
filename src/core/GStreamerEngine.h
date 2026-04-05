/**
 * @file GStreamerEngine.h
 * @brief GStreamer-based video playback and recording engine
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef GSTREAMER_ENGINE_H
#define GSTREAMER_ENGINE_H

#include <QObject>
#include <QString>
#include <QWindow>
#include <QImage>
#include <QtGlobal>

#ifndef ANDROID
#include <gst/gst.h>
#include <gst/video/video.h>
#endif

enum class PlayerState {
    Stopped,
    Playing,
    Paused,
    Error
};

struct StreamInfo {
    int width = 0;
    int height = 0;
    int fps = 0;
    int bitrate = 0;
};

class GStreamerEngine : public QObject {
    Q_OBJECT

public:
    GStreamerEngine(QObject *parent = nullptr);
    ~GStreamerEngine();

    void play(const QString &uri);
    void pause();
    void stop();
    void setVolume(int volume);
    void setNetworkLatency(int latencyMs);
    void setWindowHandle(WId windowId);

    bool startRecording(const QString &filepath);
    void stopRecording();
    void pauseRecording();
    void resumeRecording();

    bool isPlaying() const;
    bool isPaused() const;
    bool isRecording() const;
    StreamInfo getStreamInfo() const;
    int getVolume() const;

    // Screenshot functionality
    QImage captureFrame();

signals:
    void stateChanged(PlayerState state);
    void errorOccurred(const QString &error);
    void streamInfoChanged(int width, int height, int fps, int bitrate);
    void latencyChanged(int latencyMs);
    void recordingStatusChanged(bool recording, qint64 duration, qint64 fileSize);

private:
#ifndef ANDROID
    static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer data);
    static GstPadProbeReturn videoProbeCallback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
    void handleBusMessage(GstMessage *msg);
    void handleVideoBuffer(GstPad *pad, GstBuffer *buffer);
    void extractStreamInfo(GstCaps *caps);
    void doStartRecording(const QString &filepath);
    void doStopRecording();
#else
    static int busCallback(void *bus, void *msg, void *data);
    void handleBusMessage(void *msg);
    void extractStreamInfo(void *caps);
#endif
    QString buildPipeline(const QString &uri);
    void setupPipeline(const QString &uri);
    void cleanupPipeline();

#ifndef ANDROID
    GstElement *pipeline = nullptr;
    GstElement *videoSink = nullptr;
    GstElement *videoSinkBin = nullptr;
    GstElement *tee = nullptr;
    GstElement *fileSink = nullptr;
    GstElement *recordingQueue = nullptr;
    GstElement *muxer = nullptr;
    GstPad *recordingTeePad = nullptr;
    GstPad *videoProbePad = nullptr;
    gulong videoProbeId = 0;
    GstClockTime statsWindowStartNs = 0;
    GstClockTime statsLastLogNs = 0;
    quint64 statsFrameCount = 0;
    quint64 statsByteCount = 0;
    GstClockTime frameLastArrivalNs = 0;
    GstClockTime frameGapMaxNs = 0;
    quint64 frameGapSpikeCount = 0;
    GstClockTime qosLastLogNs = 0;
    quint64 qosLastProcessed = 0;
    quint64 qosLastDropped = 0;
    quint64 qosProcessedTotal = 0;
    quint64 qosDroppedTotal = 0;
#else
    void *pipeline = nullptr;
    void *videoSink = nullptr;
    void *videoSinkBin = nullptr;
    void *tee = nullptr;
    void *fileSink = nullptr;
    void *recordingQueue = nullptr;
    void *muxer = nullptr;
    void *recordingTeePad = nullptr;
#endif

    PlayerState currentState = PlayerState::Stopped;
    StreamInfo streamInfo;
    bool isRecordingActive = false;
    QString currentUri;
    QString currentRecordingPath;
    WId windowHandle = 0;
    int currentVolume = 50;
    int networkLatencyMs = 200;
};

#endif // GSTREAMER_ENGINE_H





