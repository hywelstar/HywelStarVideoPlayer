/**
 * @file GStreamerEngine.cpp
 * @brief GStreamer video engine implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "GStreamerEngine.h"
#include "utils/Logger.h"
#include <QDebug>
#include <QUrl>
#include <QFile>
#include <QtGlobal>

#ifndef ANDROID
#include <gst/app/gstappsink.h>
#include <gst/video/videooverlay.h>
#endif

GStreamerEngine::GStreamerEngine(QObject *parent)
    : QObject(parent)
{
#ifndef ANDROID
    Logger::instance().info("GStreamerEngine: Initializing GStreamer...");
    gst_init(nullptr, nullptr);

    // Log available plugins for debugging
    GstRegistry *registry = gst_registry_get();
    GstPluginFeature *feature = gst_registry_lookup_feature(registry, "rtspsrc");
    if (feature) {
        Logger::instance().info("GStreamerEngine: rtspsrc element found");
        gst_object_unref(feature);
    } else {
        Logger::instance().warning("GStreamerEngine: rtspsrc element NOT found - RTSP streams will not work");
    }

    Logger::instance().info("GStreamerEngine: GStreamer initialized successfully");
#else
    Logger::instance().info("GStreamerEngine: Running on Android (stub mode)");
#endif
}

GStreamerEngine::~GStreamerEngine() {
#ifndef ANDROID
    Logger::instance().info("GStreamerEngine: Shutting down...");
    cleanupPipeline();
    gst_deinit();
    Logger::instance().info("GStreamerEngine: Shutdown complete");
#endif
}

void GStreamerEngine::setWindowHandle(WId windowId) {
    windowHandle = windowId;
#ifndef ANDROID
    if (videoSink && GST_IS_VIDEO_OVERLAY(videoSink)) {
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videoSink), windowHandle);
    }
#endif
}

void GStreamerEngine::play(const QString &uri) {
    if (!uri.isEmpty()) {
        currentUri = uri;
    }

    if (currentUri.isEmpty()) {
        Logger::instance().error("GStreamerEngine: No URI specified");
        emit errorOccurred("No URI specified");
        return;
    }

    Logger::instance().info(QString("GStreamerEngine: Playing URI: %1").arg(currentUri));

#ifndef ANDROID
    if (pipeline) {
        cleanupPipeline();
    }

    setupPipeline(currentUri);

    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        currentState = PlayerState::Playing;
        Logger::instance().info("GStreamerEngine: Playback started");
        emit stateChanged(currentState);
    }
#else
    // Android stub: just emit state change
    currentState = PlayerState::Playing;
    emit stateChanged(currentState);
#endif
}

void GStreamerEngine::pause() {
#ifndef ANDROID
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        currentState = PlayerState::Paused;
        Logger::instance().info("GStreamerEngine: Playback paused");
        emit stateChanged(currentState);
    }
#else
    currentState = PlayerState::Paused;
    emit stateChanged(currentState);
#endif
}

void GStreamerEngine::stop() {
#ifndef ANDROID
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        currentState = PlayerState::Stopped;
        Logger::instance().info("GStreamerEngine: Playback stopped");
        emit stateChanged(currentState);
    }
    cleanupPipeline();
#else
    currentState = PlayerState::Stopped;
    emit stateChanged(currentState);
#endif
}

void GStreamerEngine::setVolume(int volume) {
#ifndef ANDROID
    currentVolume = qBound(0, volume, 100);
    if (pipeline) {
        gdouble vol = currentVolume / 100.0;
        g_object_set(G_OBJECT(pipeline), "volume", vol, nullptr);
    }
#else
    // Android stub: volume control to be implemented
    currentVolume = qBound(0, volume, 100);
#endif
}

void GStreamerEngine::startRecording(const QString &filepath) {
#ifndef ANDROID
    if (!pipeline || isRecordingActive) {
        Logger::instance().warning("GStreamerEngine: Cannot start recording - pipeline not ready or already recording");
        return;
    }

    if (!tee) {
        Logger::instance().error("GStreamerEngine: Tee element not available for recording");
        emit errorOccurred("Tee element not available for recording");
        return;
    }

    // Mark as recording immediately to prevent double-start
    isRecordingActive = true;
    currentRecordingPath = filepath;

    doStartRecording(filepath);
#else
    // Android stub: recording to be implemented
    (void)filepath;
    isRecordingActive = true;
    emit recordingStatusChanged(true, 0, 0);
#endif
}

#ifndef ANDROID
void GStreamerEngine::doStartRecording(const QString &filepath) {
    Logger::instance().info(QString("GStreamerEngine: Starting recording to: %1").arg(filepath));
    auto fail = [this](const QString &message) {
        isRecordingActive = false;
        QMetaObject::invokeMethod(this, [this, message]() {
            emit errorOccurred(message);
            emit recordingStatusChanged(false, 0, 0);
        }, Qt::QueuedConnection);
    };

    // Create recording branch elements:
    // tee -> queue -> videoconvert -> x264enc -> muxer -> filesink
    recordingQueue = gst_element_factory_make("queue", "recordingqueue");
    if (!recordingQueue) {
        Logger::instance().error("GStreamerEngine: Failed to create recording queue");
        fail("Failed to create recording queue");
        return;
    }

    GstElement *videoconvert = gst_element_factory_make("videoconvert", "recvideoconvert");
    if (!videoconvert) {
        Logger::instance().error("GStreamerEngine: Failed to create videoconvert");
        gst_object_unref(recordingQueue);
        recordingQueue = nullptr;
        fail("Failed to create videoconvert");
        return;
    }

    // Use x264enc for encoding (software encoder, widely available)
    GstElement *encoder = gst_element_factory_make("x264enc", "encoder");
    if (!encoder) {
        // Fallback to openh264enc if x264enc not available
        encoder = gst_element_factory_make("openh264enc", "encoder");
    }
    if (!encoder) {
        Logger::instance().error("GStreamerEngine: Failed to create video encoder (x264enc or openh264enc)");
        gst_object_unref(recordingQueue);
        gst_object_unref(videoconvert);
        recordingQueue = nullptr;
        fail("Failed to create video encoder");
        return;
    }

    // Configure encoder for good quality/speed balance
    g_object_set(G_OBJECT(encoder),
                 "tune", 0x00000004,  // zerolatency
                 "speed-preset", 2,   // superfast
                 nullptr);

    muxer = gst_element_factory_make("matroskamux", "muxer");
    if (!muxer) {
        Logger::instance().error("GStreamerEngine: Failed to create muxer");
        gst_object_unref(recordingQueue);
        gst_object_unref(videoconvert);
        gst_object_unref(encoder);
        recordingQueue = nullptr;
        fail("Failed to create muxer");
        return;
    }

    fileSink = gst_element_factory_make("filesink", "filesink");
    if (!fileSink) {
        Logger::instance().error("GStreamerEngine: Failed to create filesink");
        gst_object_unref(recordingQueue);
        gst_object_unref(videoconvert);
        gst_object_unref(encoder);
        gst_object_unref(muxer);
        recordingQueue = nullptr;
        muxer = nullptr;
        fail("Failed to create filesink");
        return;
    }

    g_object_set(G_OBJECT(fileSink), "location", filepath.toStdString().c_str(), nullptr);

    // Add all elements to videoSinkBin (where tee lives)
    gst_bin_add_many(GST_BIN(videoSinkBin), recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr);

    // Link recording branch: queue -> videoconvert -> encoder -> muxer -> filesink
    if (!gst_element_link_many(recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr)) {
        Logger::instance().error("GStreamerEngine: Failed to link recording elements");
        gst_bin_remove_many(GST_BIN(videoSinkBin), recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr);
        recordingQueue = nullptr;
        muxer = nullptr;
        fileSink = nullptr;
        fail("Failed to link recording elements");
        return;
    }

    // Get request pad from tee and link to recording queue
    GstPad *teeSrcPad = gst_element_request_pad_simple(tee, "src_%u");
    if (!teeSrcPad) {
        Logger::instance().error("GStreamerEngine: Failed to get tee src pad");
        gst_bin_remove_many(GST_BIN(videoSinkBin), recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr);
        recordingQueue = nullptr;
        muxer = nullptr;
        fileSink = nullptr;
        fail("Failed to get tee src pad");
        return;
    }

    GstPad *queueSinkPad = gst_element_get_static_pad(recordingQueue, "sink");
    if (!queueSinkPad) {
        Logger::instance().error("GStreamerEngine: Failed to get queue sink pad");
        gst_element_release_request_pad(tee, teeSrcPad);
        gst_object_unref(teeSrcPad);
        gst_bin_remove_many(GST_BIN(videoSinkBin), recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr);
        recordingQueue = nullptr;
        muxer = nullptr;
        fileSink = nullptr;
        fail("Failed to get queue sink pad");
        return;
    }

    // Store the tee pad for later release
    recordingTeePad = teeSrcPad;

    GstPadLinkReturn linkResult = gst_pad_link(teeSrcPad, queueSinkPad);
    gst_object_unref(queueSinkPad);

    if (linkResult != GST_PAD_LINK_OK) {
        Logger::instance().error(QString("GStreamerEngine: Failed to link tee to recording queue: %1").arg(linkResult));
        gst_element_release_request_pad(tee, teeSrcPad);
        gst_object_unref(teeSrcPad);
        recordingTeePad = nullptr;
        gst_bin_remove_many(GST_BIN(videoSinkBin), recordingQueue, videoconvert, encoder, muxer, fileSink, nullptr);
        recordingQueue = nullptr;
        muxer = nullptr;
        fileSink = nullptr;
        fail("Failed to link tee to recording queue");
        return;
    }

    // Sync state with parent pipeline
    gst_element_sync_state_with_parent(recordingQueue);
    gst_element_sync_state_with_parent(videoconvert);
    gst_element_sync_state_with_parent(encoder);
    gst_element_sync_state_with_parent(muxer);
    gst_element_sync_state_with_parent(fileSink);

    Logger::instance().info("GStreamerEngine: Recording started successfully");
    QMetaObject::invokeMethod(this, [this]() {
        emit recordingStatusChanged(true, 0, 0);
    }, Qt::QueuedConnection);
}
#endif

void GStreamerEngine::stopRecording() {
#ifndef ANDROID
    if (!isRecordingActive || !pipeline) {
        return;
    }

    Logger::instance().info("GStreamerEngine: Stopping recording...");

    // Mark as not recording immediately
    isRecordingActive = false;

    doStopRecording();
#else
    isRecordingActive = false;
    emit recordingStatusChanged(false, 0, 0);
#endif
}

#ifndef ANDROID
void GStreamerEngine::doStopRecording() {
    // Release the tee pad first
    if (tee && recordingTeePad) {
        gst_element_release_request_pad(tee, recordingTeePad);
        gst_object_unref(recordingTeePad);
        recordingTeePad = nullptr;
    }

    // Send EOS to muxer to properly finalize the file
    if (muxer) {
        gst_element_send_event(muxer, gst_event_new_eos());
    }

    // Get all recording elements by name and remove them
    GstElement *recQueue = gst_bin_get_by_name(GST_BIN(videoSinkBin), "recordingqueue");
    GstElement *recVideoConvert = gst_bin_get_by_name(GST_BIN(videoSinkBin), "recvideoconvert");
    GstElement *recEncoder = gst_bin_get_by_name(GST_BIN(videoSinkBin), "encoder");
    GstElement *recMuxer = gst_bin_get_by_name(GST_BIN(videoSinkBin), "muxer");
    GstElement *recFileSink = gst_bin_get_by_name(GST_BIN(videoSinkBin), "filesink");

    // Set to NULL state and remove from videoSinkBin
    if (recQueue) {
        gst_element_set_state(recQueue, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(videoSinkBin), recQueue);
        gst_object_unref(recQueue);
    }
    if (recVideoConvert) {
        gst_element_set_state(recVideoConvert, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(videoSinkBin), recVideoConvert);
        gst_object_unref(recVideoConvert);
    }
    if (recEncoder) {
        gst_element_set_state(recEncoder, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(videoSinkBin), recEncoder);
        gst_object_unref(recEncoder);
    }
    if (recMuxer) {
        gst_element_set_state(recMuxer, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(videoSinkBin), recMuxer);
        gst_object_unref(recMuxer);
    }
    if (recFileSink) {
        gst_element_set_state(recFileSink, GST_STATE_NULL);
        gst_bin_remove(GST_BIN(videoSinkBin), recFileSink);
        gst_object_unref(recFileSink);
    }

    recordingQueue = nullptr;
    muxer = nullptr;
    fileSink = nullptr;

    Logger::instance().info(QString("GStreamerEngine: Recording stopped, saved to: %1").arg(currentRecordingPath));
    QString savedPath = currentRecordingPath;
    currentRecordingPath.clear();

    QMetaObject::invokeMethod(this, [this]() {
        emit recordingStatusChanged(false, 0, 0);
    }, Qt::QueuedConnection);
}
#endif

void GStreamerEngine::pauseRecording() {
    // TODO: Implement pause recording
    Logger::instance().debug("GStreamerEngine: pauseRecording() called (not implemented)");
}

void GStreamerEngine::resumeRecording() {
    // TODO: Implement resume recording
    Logger::instance().debug("GStreamerEngine: resumeRecording() called (not implemented)");
}

bool GStreamerEngine::isPlaying() const {
    return currentState == PlayerState::Playing;
}

bool GStreamerEngine::isRecording() const {
    return isRecordingActive;
}

int GStreamerEngine::getVolume() const {
    return currentVolume;
}

QImage GStreamerEngine::captureFrame() {
#ifndef ANDROID
    if (!pipeline || !tee) {
        Logger::instance().warning("GStreamerEngine: Cannot capture frame - no pipeline or tee");
        return QImage();
    }

    // Create a temporary pipeline branch to capture a frame in RGB format
    // tee -> queue -> videoconvert -> appsink (with RGB format)
    GstElement *queue = gst_element_factory_make("queue", "screenshot_queue");
    GstElement *videoconvert = gst_element_factory_make("videoconvert", "screenshot_convert");
    GstElement *appsink = gst_element_factory_make("appsink", "screenshot_sink");

    if (!queue || !videoconvert || !appsink) {
        Logger::instance().error("GStreamerEngine: Failed to create screenshot elements");
        if (queue) gst_object_unref(queue);
        if (videoconvert) gst_object_unref(videoconvert);
        if (appsink) gst_object_unref(appsink);
        return QImage();
    }

    // Configure appsink to output RGB format
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "RGB",
                                        nullptr);
    g_object_set(appsink,
                 "caps", caps,
                 "max-buffers", 1,
                 "drop", TRUE,
                 "sync", FALSE,
                 nullptr);
    gst_caps_unref(caps);

    // Add elements to videoSinkBin (where tee lives)
    gst_bin_add_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);

    // Link elements
    if (!gst_element_link_many(queue, videoconvert, appsink, nullptr)) {
        Logger::instance().error("GStreamerEngine: Failed to link screenshot elements");
        gst_bin_remove_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);
        return QImage();
    }

    // Get request pad from tee
    GstPad *teeSrcPad = gst_element_request_pad_simple(tee, "src_%u");
    if (!teeSrcPad) {
        Logger::instance().error("GStreamerEngine: Failed to get tee pad for screenshot");
        gst_bin_remove_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);
        return QImage();
    }

    GstPad *queueSinkPad = gst_element_get_static_pad(queue, "sink");
    if (!queueSinkPad) {
        Logger::instance().error("GStreamerEngine: Failed to get screenshot queue sink pad");
        gst_element_release_request_pad(tee, teeSrcPad);
        gst_object_unref(teeSrcPad);
        gst_bin_remove_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);
        return QImage();
    }

    if (gst_pad_link(teeSrcPad, queueSinkPad) != GST_PAD_LINK_OK) {
        Logger::instance().error("GStreamerEngine: Failed to link tee to screenshot queue");
        gst_element_release_request_pad(tee, teeSrcPad);
        gst_object_unref(teeSrcPad);
        gst_object_unref(queueSinkPad);
        gst_bin_remove_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);
        return QImage();
    }
    gst_object_unref(queueSinkPad);

    // Sync state with parent
    gst_element_sync_state_with_parent(queue);
    gst_element_sync_state_with_parent(videoconvert);
    gst_element_sync_state_with_parent(appsink);

    // Pull sample from appsink
    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), GST_MSECOND * 120);

    QImage image;
    if (sample) {
        GstCaps *sampleCaps = gst_sample_get_caps(sample);
        GstStructure *structure = gst_caps_get_structure(sampleCaps, 0);

        int width = 0, height = 0;
        gst_structure_get_int(structure, "width", &width);
        gst_structure_get_int(structure, "height", &height);

        GstBuffer *buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;

        if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            // Create QImage from RGB data
            image = QImage(map.data, width, height, width * 3, QImage::Format_RGB888).copy();
            gst_buffer_unmap(buffer, &map);
            Logger::instance().debug(QString("GStreamerEngine: Screenshot captured %1x%2").arg(width).arg(height));
        }

        gst_sample_unref(sample);
    } else {
        Logger::instance().warning("GStreamerEngine: No sample received for screenshot");
    }

    // Cleanup: release tee pad and remove elements
    gst_element_release_request_pad(tee, teeSrcPad);
    gst_object_unref(teeSrcPad);

    gst_element_set_state(appsink, GST_STATE_NULL);
    gst_element_set_state(videoconvert, GST_STATE_NULL);
    gst_element_set_state(queue, GST_STATE_NULL);

    gst_bin_remove_many(GST_BIN(videoSinkBin), queue, videoconvert, appsink, nullptr);

    return image;
#else
    return QImage();
#endif
}

StreamInfo GStreamerEngine::getStreamInfo() const {
    return streamInfo;
}

#ifndef ANDROID
gboolean GStreamerEngine::busCallback(GstBus *bus, GstMessage *msg, gpointer data) {
    Q_UNUSED(bus)
    GStreamerEngine *engine = static_cast<GStreamerEngine *>(data);
    engine->handleBusMessage(msg);
    return TRUE;
}
#else
int GStreamerEngine::busCallback(void *bus, void *msg, void *data) {
    (void)bus;
    (void)msg;
    (void)data;
    return 1;
}
#endif

#ifndef ANDROID
void GStreamerEngine::handleBusMessage(GstMessage *msg) {
    auto refreshStreamInfoFromSink = [this]() {
        if (!videoSink) {
            return;
        }

        GstPad *sinkPad = gst_element_get_static_pad(videoSink, "sink");
        if (!sinkPad) {
            return;
        }

        GstCaps *caps = gst_pad_get_current_caps(sinkPad);
        if (caps) {
            extractStreamInfo(caps);
            gst_caps_unref(caps);
        }
        gst_object_unref(sinkPad);
    };

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError *err = nullptr;
        gchar *debug = nullptr;
        gst_message_parse_error(msg, &err, &debug);
        QString errorMsg = QString::fromUtf8(err->message);
        Logger::instance().error(QString("GStreamerEngine: Pipeline error: %1").arg(errorMsg));
        emit errorOccurred(errorMsg);
        g_error_free(err);
        g_free(debug);
        currentState = PlayerState::Error;
        emit stateChanged(currentState);
        break;
    }
    case GST_MESSAGE_EOS:
        Logger::instance().info("GStreamerEngine: End of stream reached");
        currentState = PlayerState::Stopped;
        emit stateChanged(currentState);
        break;
    case GST_MESSAGE_STATE_CHANGED: {
        GstState oldState, newState, pendingState;
        gst_message_parse_state_changed(msg, &oldState, &newState, &pendingState);
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline) && newState == GST_STATE_PLAYING) {
            refreshStreamInfoFromSink();
        }
        Q_UNUSED(oldState)
        Q_UNUSED(pendingState)
        break;
    }
    case GST_MESSAGE_ASYNC_DONE:
        refreshStreamInfoFromSink();
        break;
    case GST_MESSAGE_TAG: {
        GstTagList *tags = nullptr;
        gst_message_parse_tag(msg, &tags);
        if (tags) {
            guint bitrate = 0;
            if (gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &bitrate) && bitrate > 0) {
                streamInfo.bitrate = static_cast<int>(bitrate);
                emit streamInfoChanged(streamInfo.width, streamInfo.height, streamInfo.fps, streamInfo.bitrate);
            }
            gst_tag_list_unref(tags);
        }
        break;
    }
    case GST_MESSAGE_ELEMENT: {
        if (gst_structure_has_name(gst_message_get_structure(msg), "prepare-window-handle")) {
            if (videoSink && GST_IS_VIDEO_OVERLAY(videoSink)) {
                gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videoSink), windowHandle);
            }
        }
        break;
    }
    default:
        break;
    }
}
#else
void GStreamerEngine::handleBusMessage(void *msg) {
    (void)msg;
}
#endif

#ifndef ANDROID
void GStreamerEngine::extractStreamInfo(GstCaps *caps) {
    if (!caps) return;

    GstStructure *structure = gst_caps_get_structure(caps, 0);
    if (!structure) return;

    gint width = 0, height = 0, fps_num = 0, fps_den = 1;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);
    gst_structure_get_fraction(structure, "framerate", &fps_num, &fps_den);

    streamInfo.width = width;
    streamInfo.height = height;
    streamInfo.fps = fps_den > 0 ? fps_num / fps_den : 0;
    streamInfo.bitrate = 0; // Will be updated from other sources

    emit streamInfoChanged(streamInfo.width, streamInfo.height, streamInfo.fps, streamInfo.bitrate);
}
#else
void GStreamerEngine::extractStreamInfo(void *caps) {
    (void)caps;
}
#endif

void GStreamerEngine::setupPipeline(const QString &uri) {
#ifndef ANDROID
    Logger::instance().info(QString("GStreamerEngine: Setting up pipeline for: %1").arg(uri));

    // Create playbin3 element directly (not via gst_parse_launch)
    pipeline = gst_element_factory_make("playbin3", "playbin");
    if (!pipeline) {
        // Fallback to playbin
        pipeline = gst_element_factory_make("playbin", "playbin");
    }
    if (!pipeline) {
        Logger::instance().error("GStreamerEngine: Failed to create playbin3/playbin");
        emit errorOccurred("Failed to create playbin");
        return;
    }

    // Set URI
    g_object_set(pipeline, "uri", uri.toStdString().c_str(), nullptr);
    g_object_set(G_OBJECT(pipeline), "volume", currentVolume / 100.0, nullptr);

    // Build video sink bin: videoconvert ! tee ! queue ! d3d11videosink
    videoSinkBin = gst_bin_new("videosinkbin");
    GstElement *videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    tee = gst_element_factory_make("tee", "t");
    GstElement *queue = gst_element_factory_make("queue", "displayqueue");
    videoSink = gst_element_factory_make("d3d11videosink", "videosink");

    if (!videoconvert || !tee || !queue || !videoSink) {
        Logger::instance().error("GStreamerEngine: Failed to create video sink elements");
        emit errorOccurred("Failed to create video sink elements");
        gst_object_unref(pipeline);
        pipeline = nullptr;
        tee = nullptr;
        videoSink = nullptr;
        if (videoconvert) gst_object_unref(videoconvert);
        if (tee) gst_object_unref(tee);
        if (queue) gst_object_unref(queue);
        if (videoSink) gst_object_unref(videoSink);
        gst_object_unref(videoSinkBin);
        return;
    }

    gst_bin_add_many(GST_BIN(videoSinkBin), videoconvert, tee, queue, videoSink, nullptr);
    if (!gst_element_link(videoconvert, tee) ||
        !gst_element_link(tee, queue) ||
        !gst_element_link(queue, videoSink)) {
        Logger::instance().error("GStreamerEngine: Failed to link video sink elements");
        emit errorOccurred("Failed to link video sink elements");
        gst_object_unref(pipeline);
        pipeline = nullptr;
        videoSink = nullptr;
        videoSinkBin = nullptr;
        tee = nullptr;
        return;
    }

    // Create ghost pad so playbin can link to this bin
    GstPad *sinkPad = gst_element_get_static_pad(videoconvert, "sink");
    gst_element_add_pad(videoSinkBin, gst_ghost_pad_new("sink", sinkPad));
    gst_object_unref(sinkPad);

    // Set window handle
    if (windowHandle != 0) {
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videoSink), windowHandle);
        Logger::instance().debug(QString("GStreamerEngine: Window handle set: %1").arg(windowHandle));
    }

    // Set video-sink on playbin
    g_object_set(pipeline, "video-sink", videoSinkBin, nullptr);

    Logger::instance().info("GStreamerEngine: Pipeline created successfully");

    // Setup bus
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, busCallback, this);
    gst_object_unref(bus);
    Logger::instance().debug("GStreamerEngine: Bus watch added");
#else
    (void)uri;
#endif
}

void GStreamerEngine::cleanupPipeline() {
#ifndef ANDROID
    if (isRecordingActive) {
        stopRecording();
    }

    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
        videoSink = nullptr;
        videoSinkBin = nullptr;
        tee = nullptr;
    }
#endif
}

QString GStreamerEngine::buildPipeline(const QString &uri) {
#ifndef ANDROID
    // Use playbin3 for automatic audio/video handling
    // This supports all formats: video, audio, or both
    // Video sink: d3d11videosink on Windows for hardware acceleration
    // Audio sink: autoaudiosink (auto-detect best audio output)

    QString videoSinkName = "d3d11videosink";

    // playbin3 handles audio automatically with default audio sink
    // Only override video-sink for tee support (recording)
    return QString("playbin3 uri=\"%1\" name=playbin "
                  "video-sink=\"videoconvert ! tee name=t ! queue name=displayqueue ! %2 name=videosink\"")
        .arg(uri, videoSinkName);
#else
    (void)uri;
    return "";
#endif
}
