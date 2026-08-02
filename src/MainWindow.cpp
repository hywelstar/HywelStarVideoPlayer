/**
 * @file MainWindow.cpp
 * @brief Main application window implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "MainWindow.h"
#include "ui/VideoDisplayWidget.h"
#include "ui/ControlBar.h"
#include "ui/StatusBar.h"
#include "ui/QuickConnectBar.h"
#include "ui/LocalFileListWidget.h"
#include "ui/SettingsDialog.h"
#include "ui/AboutDialog.h"
#include "ui/ThemeManager.h"
#include "core/GStreamerEngine.h"
#include "core/RecordingManager.h"
#include "core/ConfigManager.h"
#include "utils/Logger.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QSettings>
#include <QMenuBar>
#include <QAction>
#include <QSlider>
#include <QSplitter>
#include <QUrl>
#include <QtGlobal>

namespace {
constexpr int kMinNetworkLatencyMs = 0;
constexpr int kMaxNetworkLatencyMs = 5000;
constexpr int kDefaultNetworkLatencyMs = 0;

bool isLocalFileUri(const QString &uri) {
    return uri.startsWith("file://", Qt::CaseInsensitive);
}

bool isValidStreamUri(const QString &uri) {
    if (uri.isEmpty()) {
        return false;
    }

    const QUrl url(uri);
    if (!url.isValid()) {
        return false;
    }

    const QString scheme = url.scheme().toLower();
    return scheme == "rtsp" || scheme == "udp" || scheme == "tcp" ||
           scheme == "http" || scheme == "https" ||
           scheme == "rtmp" || scheme == "rtmps" || scheme == "rtmpt" ||
           scheme == "srt" || scheme == "file";
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoWidget(std::make_unique<VideoDisplayWidget>())
    , controlBar(std::make_unique<ControlBar>())
    , statusBar(std::make_unique<StatusBar>())
    , quickConnectBar(std::make_unique<QuickConnectBar>())
    , localFileList(std::make_unique<LocalFileListWidget>())
    , gstreamerEngine(std::make_unique<GStreamerEngine>())
    , recordingManager(std::make_unique<RecordingManager>())
    , configManager(std::make_unique<ConfigManager>())
    , recordingTimer(new QTimer(this))
    , playbackTimer(new QTimer(this))
    , overlayHideTimer(new QTimer(this))
    , clickTimer(new QTimer(this))
{
    Logger::instance().info("MainWindow: Initializing main window...");
    QSettings initialSettings("HywelStar", "HywelStarVideoPlayer");
    ThemeManager::applyApplicationTheme(
        ThemeManager::modeFromString(initialSettings.value("themeMode", ThemeManager::modeToString(ThemeMode::System)).toString()));
    setWindowTitle("HywelStar Video Player");
    setWindowIcon(QIcon(":/icons/app_icon"));
    resize(1280, 720);

    setupMenuBar();
    setupUI();
    connectSignals();
    loadSettings();

    // Setup recording timer
    connect(recordingTimer, &QTimer::timeout, this, &MainWindow::updateRecordingTime);
    connect(playbackTimer, &QTimer::timeout, this, &MainWindow::updatePlaybackPosition);
    playbackTimer->start(500);
    overlayHideTimer->setSingleShot(true);
    connect(overlayHideTimer, &QTimer::timeout, this, &MainWindow::hidePlaybackOverlay);
    clickTimer->setSingleShot(true);
    connect(clickTimer, &QTimer::timeout, this, &MainWindow::onPlayPause);
    qApp->installEventFilter(this);

    Logger::instance().info("MainWindow: Initialization complete");
}

MainWindow::~MainWindow() {
    Logger::instance().info("MainWindow: Shutting down...");
    qApp->removeEventFilter(this);
    saveSettings();
}

void MainWindow::setupMenuBar() {
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(ThemeManager::mainMenuStyle());
    QAction *settingsTopAction = menuBar->addAction(tr("Settings"));
    connect(settingsTopAction, &QAction::triggered, this, &MainWindow::onShowSettings);
    QAction *aboutTopAction = menuBar->addAction(tr("About"));
    connect(aboutTopAction, &QAction::triggered, this, &MainWindow::onShowAbout);

    setMenuBar(menuBar);
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add quick connect bar
    mainLayout->addWidget(quickConnectBar.get());

    contentSplitter = new QSplitter(Qt::Horizontal);
    contentSplitter->setChildrenCollapsible(false);
    contentSplitter->addWidget(localFileList.get());
    contentSplitter->addWidget(videoWidget.get());
    contentSplitter->setStretchFactor(0, 0);
    contentSplitter->setStretchFactor(1, 1);
    contentSplitter->setSizes({260, 1020});
    contentSplitter->setStyleSheet(ThemeManager::splitterStyle());
    mainLayout->addWidget(contentSplitter, 1);

    // Add control bar
    mainLayout->addWidget(controlBar.get());

    // Add status bar
    setStatusBar(statusBar.get());

    setCentralWidget(centralWidget);
    setupPlaybackOverlay(centralWidget);
    applyTheme();

    // Set window handle for GStreamer
    gstreamerEngine->setWindowHandle((WId)videoWidget->winId());
}

void MainWindow::connectSignals() {
    // Quick connect bar signals
    connect(quickConnectBar.get(), &QuickConnectBar::playRequested,
            this, &MainWindow::onPlayUri);
    connect(quickConnectBar.get(), &QuickConnectBar::settingsRequested,
            this, &MainWindow::onShowSettings);
    connect(quickConnectBar.get(), &QuickConnectBar::localModeChanged,
            this, &MainWindow::onPlaybackModeChanged);

    // Local file list signals
    connect(localFileList.get(), &LocalFileListWidget::playFileRequested,
            this, &MainWindow::onPlayLocalFile);

    // Control bar signals
    connect(controlBar.get(), &ControlBar::playPauseRequested,
            this, &MainWindow::onPlayPause);
    connect(controlBar.get(), &ControlBar::recordingRequested,
            this, &MainWindow::onStartRecording);
    connect(controlBar.get(), &ControlBar::recordingStopRequested,
            this, &MainWindow::onStopRecording);
    connect(controlBar.get(), &ControlBar::screenshotRequested,
            this, &MainWindow::onScreenshot);
    connect(controlBar.get(), &ControlBar::fullscreenRequested,
            this, &MainWindow::onToggleFullScreen);
    connect(controlBar.get(), &ControlBar::gridToggleRequested,
            this, &MainWindow::onToggleGrid);
    connect(controlBar.get(), &ControlBar::volumeChanged,
            this, &MainWindow::onVolumeChanged);
    connect(controlBar.get(), &ControlBar::stretchToggleRequested,
            this, &MainWindow::onStretchToggleRequested);
    connect(controlBar.get(), &ControlBar::playbackRateChanged,
            this, &MainWindow::onPlaybackRateChanged);
    connect(controlBar.get(), &ControlBar::playbackEndModeChanged,
            this, &MainWindow::onPlaybackEndModeChanged);

    // Video display widget signals
    connect(videoWidget.get(), &VideoDisplayWidget::fullScreenRequested,
            this, &MainWindow::onToggleFullScreen);
    connect(videoWidget.get(), &VideoDisplayWidget::gridToggleRequested,
            this, &MainWindow::onToggleGrid);
    connect(videoWidget.get(), &VideoDisplayWidget::seekRequested,
            this, &MainWindow::onSeekRequested);

    // GStreamer engine signals
    connect(gstreamerEngine.get(), QOverload<int, int, int, int>::of(&GStreamerEngine::streamInfoChanged),
            this, &MainWindow::onStreamInfoChanged);
    connect(gstreamerEngine.get(), &GStreamerEngine::latencyChanged,
            statusBar.get(), &StatusBar::updateLatency);
    connect(gstreamerEngine.get(), QOverload<PlayerState>::of(&GStreamerEngine::stateChanged),
            this, [this](PlayerState state) { onPlayerStateChanged((int)state); });
    connect(gstreamerEngine.get(), &GStreamerEngine::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    connect(gstreamerEngine.get(), &GStreamerEngine::recordingStatusChanged,
            this, &MainWindow::onRecordingStatusChanged);
    connect(gstreamerEngine.get(), &GStreamerEngine::endOfStream,
            this, &MainWindow::onEndOfStream);
}

void MainWindow::onPlayUri(const QString &uri) {
    if (!isValidStreamUri(uri)) {
        statusBar->showError("Invalid stream URI");
        Logger::instance().warning(QString("MainWindow: Invalid URI: %1").arg(uri));
        return;
    }
    Logger::instance().info(QString("MainWindow: Playing URI: %1").arg(uri));
    currentStreamUri = uri;
    currentPlaybackIsLocal = false;
    currentLocalFilePath.clear();
    streamStoppedForPause = false;
    localEndHandled = false;
    localFileList->setNowPlayingFilePath(QString());
    gstreamerEngine->play(uri);
    controlBar->setPlaybackState(PlaybackState::Playing);
}

void MainWindow::onPlayLocalFile(const QString &filePath) {
    playLocalFilePath(filePath, false);
}

void MainWindow::onPlaybackModeChanged(bool localMode) {
    Logger::instance().info(QString("MainWindow: Playback mode changed to %1, stopping current playback")
                            .arg(localMode ? "Local" : "Stream"));

    hidePlaybackOverlay();
    gstreamerEngine->stop();
    controlBar->setPlaybackState(PlaybackState::Stopped);
    currentPlaybackIsLocal = false;
    currentLocalFilePath.clear();
    streamStoppedForPause = false;
    localEndHandled = false;
    pendingSeekPositionMs = -1;
    pendingSeekRefreshHoldTicks = 0;
    localFileList->setNowPlayingFilePath(QString());
    videoWidget->setPosition(-1, -1);

    setLocalFilePanelVisible(localMode);
}

void MainWindow::playLocalFilePath(const QString &filePath, bool restartPlayback) {
    currentLocalFilePath = filePath;
    currentPlaybackIsLocal = true;
    streamStoppedForPause = false;
    localEndHandled = false;
    quickConnectBar->setLocalMode(true);
    setLocalFilePanelVisible(true);
    localFileList->setNowPlayingFilePath(filePath);
    const QString uri = QUrl::fromLocalFile(filePath).toString();
    Logger::instance().info(QString("MainWindow: Playing local file: %1").arg(filePath));
    if (restartPlayback) {
        gstreamerEngine->restart(uri);
    } else {
        gstreamerEngine->play(uri);
    }
    controlBar->setPlaybackState(PlaybackState::Playing);
}

void MainWindow::onShowSettings() {
    SettingsDialog dialog(this);
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    dialog.setRecordingPath(recordingManager->getRecordingPath());
    dialog.setScreenshotPath(recordingManager->getScreenshotPath());
    dialog.setRecordingFormat(settings.value("recordingFormat", "mkv").toString().toLower());
    dialog.setNetworkLatency(qBound(kMinNetworkLatencyMs, settings.value("networkLatency", kDefaultNetworkLatencyMs).toInt(), kMaxNetworkLatencyMs));
    dialog.setThemeMode(settings.value("themeMode", ThemeManager::modeToString(ThemeMode::System)).toString());

    if (dialog.exec() == QDialog::Accepted) {
        recordingManager->setRecordingPath(dialog.getRecordingPath());
        recordingManager->setScreenshotPath(dialog.getScreenshotPath());
        settings.setValue("recordingFormat", dialog.getRecordingFormat());
        settings.setValue("themeMode", dialog.getThemeMode());
        ThemeManager::applyApplicationTheme(ThemeManager::modeFromString(dialog.getThemeMode()));
        applyTheme();
        playbackEndMode = static_cast<PlaybackEndMode>(qBound(0, dialog.getLoopMode(), 2));
        controlBar->setPlaybackEndMode(playbackEndMode);
        settings.setValue("loopMode", static_cast<int>(playbackEndMode));
        const int networkLatency = qBound(kMinNetworkLatencyMs, dialog.getNetworkLatency(), kMaxNetworkLatencyMs);
        settings.setValue("networkLatency", networkLatency);
        gstreamerEngine->setNetworkLatency(networkLatency);
        Logger::instance().info(QString("MainWindow: Settings updated (latency=%1 ms, format=%2)").arg(networkLatency).arg(dialog.getRecordingFormat()));
    }
}

void MainWindow::onShowAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onPlayPause() {
    hidePlaybackOverlay();
    if (gstreamerEngine->isPlaying()) {
        if (currentPlaybackIsLocal) {
            Logger::instance().info("MainWindow: Pause requested");
            gstreamerEngine->pause();
        } else {
            currentStreamUri = quickConnectBar->getStreamUri();
            streamStoppedForPause = true;
            Logger::instance().info(QString("MainWindow: Stream pause requested, stopping live stream: %1").arg(currentStreamUri));
            gstreamerEngine->stop();
        }
        controlBar->setPlaybackState(PlaybackState::Paused);
    } else if (streamStoppedForPause) {
        const QString uri = currentStreamUri.isEmpty() ? quickConnectBar->getStreamUri() : currentStreamUri;
        Logger::instance().info(QString("MainWindow: Stream resume requested, reconnecting: %1").arg(uri));
        if (!uri.isEmpty() && isValidStreamUri(uri)) {
            streamStoppedForPause = false;
            onPlayUri(uri);
        } else if (!uri.isEmpty()) {
            statusBar->showError("Invalid stream URI");
        }
    } else if (gstreamerEngine->isPaused()) {
        Logger::instance().info("MainWindow: Resume requested");
        gstreamerEngine->resume();
        controlBar->setPlaybackState(PlaybackState::Playing);
        QTimer::singleShot(0, this, [this]() {
            gstreamerEngine->refreshVideo();
        });
        QTimer::singleShot(120, this, [this]() {
            gstreamerEngine->refreshVideo();
        });
    } else {
        Logger::instance().info("MainWindow: Play requested");
        QString uri = quickConnectBar->getStreamUri();
        if (!uri.isEmpty() && isValidStreamUri(uri)) {
            onPlayUri(uri);
        } else if (!uri.isEmpty()) {
            statusBar->showError("Invalid stream URI");
        }
    }
}

void MainWindow::onStartRecording() {
    if (!gstreamerEngine->isPlaying()) {
        statusBar->showError("Start playback before recording");
        Logger::instance().warning("MainWindow: Recording requested while not playing");
        return;
    }

    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    QString format = settings.value("recordingFormat", "mkv").toString().toLower();
    if (format != "mkv") {
        Logger::instance().warning(QString("MainWindow: Unsupported recording format '%1', falling back to mkv").arg(format));
        statusBar->showError("Only MKV recording is currently supported");
        format = "mkv";
        settings.setValue("recordingFormat", format);
    }

    const QString filepath = recordingManager->generateFilename(format);
    Logger::instance().info(QString("MainWindow: Starting recording to: %1").arg(filepath));

    if (gstreamerEngine->startRecording(filepath)) {
        recordingManager->startRecording(format, "high", filepath);
    } else {
        Logger::instance().warning("MainWindow: Recording start request rejected");
    }
}

void MainWindow::onStopRecording() {
    Logger::instance().info("MainWindow: Stopping recording");
    gstreamerEngine->stopRecording();
}

void MainWindow::updateRecordingTime() {
    if (gstreamerEngine->isRecording()) {
        controlBar->setRecordingTime(recordingElapsed.elapsed());
    }
}

void MainWindow::updatePlaybackPosition() {
    qint64 positionMs = gstreamerEngine->positionMs();
    const qint64 durationMs = gstreamerEngine->durationMs();
    if (playbackPositionSlider && playbackPositionSlider->isSliderDown()) {
        return;
    }

    if (pendingSeekRefreshHoldTicks > 0 && pendingSeekPositionMs >= 0) {
        positionMs = pendingSeekPositionMs;
        pendingSeekRefreshHoldTicks -= 1;
    } else {
        pendingSeekPositionMs = -1;
    }

    if (currentPlaybackIsLocal && !localEndHandled && gstreamerEngine->isPlaying() &&
        durationMs > 0 && positionMs >= qMax<qint64>(0, durationMs - 250)) {
        Logger::instance().info(QString("MainWindow: Local end detected by position fallback (position=%1 ms, duration=%2 ms)")
                                .arg(positionMs)
                                .arg(durationMs));
        onEndOfStream();
    }

    videoWidget->setPosition(positionMs, durationMs);

    const bool hasDuration = durationMs > 0;
    isUpdatingPlaybackOverlay = true;
    playbackPositionSlider->setEnabled(hasDuration);
    playbackPositionSlider->setMaximum(hasDuration ? static_cast<int>(durationMs / 1000) : 0);
    if (hasDuration && !playbackPositionSlider->isSliderDown()) {
        playbackPositionSlider->setValue(static_cast<int>(qBound<qint64>(0, positionMs / 1000, durationMs / 1000)));
    }
    isUpdatingPlaybackOverlay = false;

    playbackPositionLabel->setText(positionMs >= 0 ? formatPlaybackTime(positionMs) : "00:00");
    playbackDurationLabel->setText(hasDuration ? formatPlaybackTime(durationMs) : "--:--");
    positionPlaybackOverlay();
}

void MainWindow::onScreenshot() {
    Logger::instance().info("MainWindow: Screenshot requested");

    // Capture frame from GStreamer pipeline
    QImage frame = gstreamerEngine->captureFrame();

    if (frame.isNull()) {
        Logger::instance().error("MainWindow: Failed to capture frame from GStreamer");
        statusBar->showError("Failed to capture screenshot");
        return;
    }

    // Generate filename and save
    QString filepath = recordingManager->generateScreenshotFilename();
    if (frame.save(filepath, "PNG")) {
        Logger::instance().info(QString("MainWindow: Screenshot saved to: %1").arg(filepath));
        statusBar->updateConnectionStatus(QString("Screenshot saved"));
    } else {
        Logger::instance().error(QString("MainWindow: Failed to save screenshot to: %1").arg(filepath));
        statusBar->showError("Failed to save screenshot");
    }
}

void MainWindow::onToggleFullScreen() {
    hidePlaybackOverlay();
    if (isFullScreen) {
        Logger::instance().debug("MainWindow: Exiting fullscreen");
        if (menuBar()) {
            menuBar()->show();
        }
        quickConnectBar->show();
        isFullScreen = false;
        setLocalFilePanelVisible(localFilePanelVisible);
        controlBar->show();
        statusBar->show();
        showNormal();
    } else {
        Logger::instance().debug("MainWindow: Entering fullscreen");
        if (menuBar()) {
            menuBar()->hide();
        }
        quickConnectBar->hide();
        localFileList->hide();
        controlBar->hide();
        statusBar->hide();
        showFullScreen();
        isFullScreen = true;
    }
    controlBar->setFullscreen(isFullScreen);
    QTimer::singleShot(0, this, [this]() {
        gstreamerEngine->setWindowHandle((WId)videoWidget->winId());
        gstreamerEngine->refreshVideo();
        positionPlaybackOverlay();
    });
    QTimer::singleShot(120, this, [this]() {
        gstreamerEngine->refreshVideo();
        positionPlaybackOverlay();
    });
}

void MainWindow::onToggleGrid() {
    isGridVisible = !isGridVisible;
    videoWidget->showGrid(isGridVisible);
    controlBar->setGridActive(isGridVisible);
}

void MainWindow::onVolumeChanged(int volume) {
    gstreamerEngine->setVolume(volume);
}

void MainWindow::onSeekRequested(qint64 positionMs) {
    pendingSeekPositionMs = positionMs;
    pendingSeekRefreshHoldTicks = 4;
    gstreamerEngine->seek(positionMs);
    updatePlaybackPosition();
    showPlaybackOverlay();
}

void MainWindow::onStretchToggleRequested(bool stretch) {
    gstreamerEngine->setStretchVideo(stretch);
}

void MainWindow::onPlaybackRateChanged(double rate) {
    gstreamerEngine->setPlaybackRate(rate);
}

void MainWindow::onPlaybackEndModeChanged(PlaybackEndMode mode) {
    playbackEndMode = mode;
    Logger::instance().info(QString("MainWindow: Local playback end mode changed: %1").arg(static_cast<int>(mode)));
}

void MainWindow::onStreamInfoChanged(int width, int height, int fps, int bitrate) {
    statusBar->updateStreamInfo(width, height, fps, bitrate);
}

void MainWindow::onPlayerStateChanged(int state) {
    PlayerState playerState = static_cast<PlayerState>(state);
    switch (playerState) {
    case PlayerState::Playing:
        statusBar->updateConnectionStatus("Playing");
        controlBar->setPlaybackState(PlaybackState::Playing);
        break;
    case PlayerState::Paused:
        statusBar->updateConnectionStatus("Paused");
        controlBar->setPlaybackState(PlaybackState::Paused);
        break;
    case PlayerState::Stopped:
        statusBar->updateConnectionStatus("Stopped");
        controlBar->setPlaybackState(PlaybackState::Stopped);
        break;
    case PlayerState::Error:
        statusBar->updateConnectionStatus("Error");
        controlBar->setPlaybackState(PlaybackState::Stopped);
        break;
    }
}

void MainWindow::onErrorOccurred(const QString &error) {
    Logger::instance().error(QString("MainWindow: Error occurred: %1").arg(error));
    statusBar->showError(error);
    if (!gstreamerEngine->isRecording()) {
        controlBar->setRecordingActive(false);
        statusBar->updateRecordingStatus(false);
        recordingTimer->stop();
    }
}

void MainWindow::onRecordingStatusChanged(bool recording, qint64 duration, qint64 fileSize) {
    Q_UNUSED(duration)
    Q_UNUSED(fileSize)

    statusBar->updateRecordingStatus(recording);
    controlBar->setRecordingActive(recording);

    if (recording) {
        recordingElapsed.start();
        recordingTimer->start(1000);
    } else {
        recordingTimer->stop();
        controlBar->setRecordingTime(0);
        recordingManager->stopRecording();
    }
}

void MainWindow::onEndOfStream() {
    if (localEndHandled) {
        return;
    }
    localEndHandled = true;

    Logger::instance().info(QString("MainWindow: EOS received (local=%1, mode=%2, file=%3)")
                            .arg(currentPlaybackIsLocal ? "true" : "false")
                            .arg(static_cast<int>(playbackEndMode))
                            .arg(currentLocalFilePath));
    if (!currentPlaybackIsLocal || currentLocalFilePath.isEmpty()) {
        return;
    }

    switch (playbackEndMode) {
    case PlaybackEndMode::Stop:
        Logger::instance().info("MainWindow: Local file ended, stopping playback");
        QTimer::singleShot(0, this, [this]() {
            gstreamerEngine->stop();
            controlBar->setPlaybackState(PlaybackState::Stopped);
        });
        break;
    case PlaybackEndMode::RepeatOne:
        Logger::instance().info(QString("MainWindow: Repeating local file: %1").arg(currentLocalFilePath));
        {
            const QString repeatPath = currentLocalFilePath;
            QTimer::singleShot(0, this, [this, repeatPath]() {
                playLocalFilePath(repeatPath, true);
            });
        }
        break;
    case PlaybackEndMode::RepeatAll: {
        const QString nextPath = localFileList->nextFilePath(currentLocalFilePath);
        if (nextPath.isEmpty()) {
            Logger::instance().info("MainWindow: Local list ended with no next file");
            return;
        }
        Logger::instance().info(QString("MainWindow: Playing next local file: %1").arg(nextPath));
        QTimer::singleShot(0, this, [this, nextPath]() {
            playLocalFilePath(nextPath, true);
        });
        break;
    }
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (!videoWidget || !centralWidget()) {
        return QMainWindow::eventFilter(watched, event);
    }

    QWidget *activeModal = QApplication::activeModalWidget();
    if (activeModal && activeModal != this) {
        mousePressedInVideo = false;
        suppressClickToggle = false;
        clickTimer->stop();
        return QMainWindow::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
        positionPlaybackOverlay();
        break;
    case QEvent::MouseMove: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (globalPointInVideo(mouseEvent->globalPosition().toPoint())) {
            showPlaybackOverlay();
            if (mousePressedInVideo &&
                (mouseEvent->globalPosition().toPoint() - videoMousePressGlobalPos).manhattanLength() > 6) {
                suppressClickToggle = true;
            }
        }
        break;
    }
    case QEvent::MouseButtonPress: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && globalPointInVideo(mouseEvent->globalPosition().toPoint())) {
            showPlaybackOverlay();
            mousePressedInVideo = true;
            suppressClickToggle = false;
            videoMousePressGlobalPos = mouseEvent->globalPosition().toPoint();
            if (isOverlayObject(watched)) {
                suppressClickToggle = watched == playbackPositionSlider;
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && mousePressedInVideo) {
            showPlaybackOverlay();
            const int clickDistance = (mouseEvent->globalPosition().toPoint() - videoMousePressGlobalPos).manhattanLength();
            if (!suppressClickToggle && clickDistance <= 6 && globalPointInVideo(mouseEvent->globalPosition().toPoint())) {
                Logger::instance().debug("MainWindow: Video click toggled playback");
                clickTimer->start(80);
            }
            mousePressedInVideo = false;
        }
        break;
    }
    case QEvent::MouseButtonDblClick: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && globalPointInVideo(mouseEvent->globalPosition().toPoint())) {
            clickTimer->stop();
            suppressClickToggle = true;
            showPlaybackOverlay();
            onToggleFullScreen();
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_F:
        onToggleFullScreen();
        break;
    case Qt::Key_Escape:
        if (isFullScreen) {
            onToggleFullScreen();
        }
        break;
    case Qt::Key_Space:
        onPlayPause();
        break;
    case Qt::Key_R:
        if (gstreamerEngine->isRecording()) {
            onStopRecording();
        } else {
            onStartRecording();
        }
        break;
    case Qt::Key_S:
        onScreenshot();
        break;
    case Qt::Key_G:
        onToggleGrid();
        break;
    case Qt::Key_Q:
        close();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    gstreamerEngine->stop();
    event->accept();
}

void MainWindow::loadSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");

    // Window geometry
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    ThemeManager::applyApplicationTheme(
        ThemeManager::modeFromString(settings.value("themeMode", ThemeManager::modeToString(ThemeMode::System)).toString()));
    applyTheme();

    // Last URI
    QString lastUri = settings.value("lastUri", "").toString();
    if (!lastUri.isEmpty() && !isLocalFileUri(lastUri)) {
        quickConnectBar->setUri(lastUri);
    }

    // Volume
    int volume = qBound(0, settings.value("volume", 80).toInt(), 100);
    controlBar->setVolume(volume);
    gstreamerEngine->setVolume(volume);

    const double playbackRate = settings.value("playbackRate", 1.0).toDouble();
    controlBar->setPlaybackRate(playbackRate);
    gstreamerEngine->setPlaybackRate(playbackRate);

    const bool stretchVideo = settings.value("stretchVideo", false).toBool();
    controlBar->setStretchActive(stretchVideo);
    gstreamerEngine->setStretchVideo(stretchVideo);

    playbackEndMode = static_cast<PlaybackEndMode>(
        qBound(0, settings.value("loopMode", static_cast<int>(PlaybackEndMode::Stop)).toInt(), 2));
    controlBar->setPlaybackEndMode(playbackEndMode);

    // Network latency
    int networkLatency = qBound(kMinNetworkLatencyMs, settings.value("networkLatency", kDefaultNetworkLatencyMs).toInt(), kMaxNetworkLatencyMs);
    gstreamerEngine->setNetworkLatency(networkLatency);

    // Recording paths
    QString recordingPath = settings.value("recordingPath", "").toString();
    recordingManager->setRecordingPath(recordingPath);
    QString screenshotPath = settings.value("screenshotPath", "").toString();
    recordingManager->setScreenshotPath(screenshotPath);

    // Local file list
    localFileList->loadSettings();

    const bool localMode = settings.value("localMode", false).toBool();
    quickConnectBar->setLocalMode(localMode);
    setLocalFilePanelVisible(localMode);

    Logger::instance().debug(QString("MainWindow: Settings loaded (volume=%1, latency=%2 ms)").arg(volume).arg(networkLatency));
}

void MainWindow::saveSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");

    // Window geometry
    settings.setValue("geometry", saveGeometry());

    // Last URI
    const QString currentUri = quickConnectBar->getStreamUri();
    if (!isLocalFileUri(currentUri)) {
        settings.setValue("lastUri", currentUri);
    }

    // Volume
    settings.setValue("volume", controlBar->volume());
    settings.setValue("playbackRate", gstreamerEngine->playbackRate());
    settings.setValue("stretchVideo", gstreamerEngine->stretchVideo());
    settings.setValue("loopMode", static_cast<int>(playbackEndMode));
    settings.setValue("themeMode", qApp->property("themeMode").toString());

    // Network latency
    int networkLatency = qBound(kMinNetworkLatencyMs, settings.value("networkLatency", kDefaultNetworkLatencyMs).toInt(), kMaxNetworkLatencyMs);
    settings.setValue("networkLatency", networkLatency);

    // Recording paths
    settings.setValue("recordingPath", recordingManager->getRecordingPath());
    settings.setValue("screenshotPath", recordingManager->getScreenshotPath());

    // Local file list
    localFileList->saveSettings();
    settings.setValue("localMode", quickConnectBar->isLocalMode());

    Logger::instance().debug("MainWindow: Settings saved");
}

void MainWindow::setLocalFilePanelVisible(bool visible) {
    localFilePanelVisible = visible;
    if (isFullScreen) {
        localFileList->hide();
        return;
    }

    localFileList->setVisible(visible);
    if (contentSplitter) {
        contentSplitter->setSizes(visible ? QList<int>{260, 1020} : QList<int>{0, 1280});
    }
}

void MainWindow::setupPlaybackOverlay(QWidget *parent) {
    playbackOverlay = new QWidget(parent);
    playbackOverlay->setVisible(false);
    playbackOverlay->setMouseTracking(true);
    playbackOverlay->setStyleSheet(ThemeManager::playbackOverlayStyle());

    auto *layout = new QHBoxLayout(playbackOverlay);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(10);

    playbackPositionLabel = new QLabel("00:00", playbackOverlay);
    playbackPositionLabel->setMinimumWidth(46);
    layout->addWidget(playbackPositionLabel);

    playbackPositionSlider = new QSlider(Qt::Horizontal, playbackOverlay);
    playbackPositionSlider->setMinimum(0);
    playbackPositionSlider->setMaximum(0);
    playbackPositionSlider->setEnabled(false);
    layout->addWidget(playbackPositionSlider, 1);

    playbackDurationLabel = new QLabel("--:--", playbackOverlay);
    playbackDurationLabel->setMinimumWidth(46);
    layout->addWidget(playbackDurationLabel);

    connect(playbackPositionSlider, &QSlider::sliderPressed, this, &MainWindow::showPlaybackOverlay);
    connect(playbackPositionSlider, &QSlider::sliderMoved, this, [this](int value) {
        if (!isUpdatingPlaybackOverlay) {
            playbackPositionLabel->setText(formatPlaybackTime(value * 1000LL));
        }
        showPlaybackOverlay();
    });
    connect(playbackPositionSlider, &QSlider::sliderReleased, this, [this]() {
        onSeekRequested(playbackPositionSlider->value() * 1000LL);
    });

    positionPlaybackOverlay();
}

void MainWindow::applyTheme() {
    if (menuBar()) {
        menuBar()->setStyleSheet(ThemeManager::mainMenuStyle());
    }
    if (contentSplitter) {
        contentSplitter->setStyleSheet(ThemeManager::splitterStyle());
    }
    if (quickConnectBar) {
        quickConnectBar->applyTheme();
    }
    if (localFileList) {
        localFileList->applyTheme();
    }
    if (controlBar) {
        controlBar->applyTheme();
    }
    if (statusBar) {
        statusBar->applyTheme();
    }
    if (playbackOverlay) {
        playbackOverlay->setStyleSheet(ThemeManager::playbackOverlayStyle());
    }
}

void MainWindow::positionPlaybackOverlay() {
    if (!playbackOverlay || !centralWidget() || !videoWidget) {
        return;
    }

    const QPoint videoTopLeft = centralWidget()->mapFromGlobal(videoWidget->mapToGlobal(QPoint(0, 0)));
    const QSize videoSize = videoWidget->size();
    const int margin = 18;
    const int overlayHeight = 42;
    playbackOverlay->setGeometry(videoTopLeft.x() + margin,
                                 videoTopLeft.y() + qMax(margin, videoSize.height() - overlayHeight - margin),
                                 qMax(0, videoSize.width() - margin * 2),
                                 overlayHeight);
}

void MainWindow::showPlaybackOverlay() {
    if (!playbackOverlay) {
        return;
    }

    positionPlaybackOverlay();
    playbackOverlay->show();
    playbackOverlay->raise();
    overlayHideTimer->start(3000);
}

void MainWindow::hidePlaybackOverlay() {
    if (playbackPositionSlider && playbackPositionSlider->isSliderDown()) {
        showPlaybackOverlay();
        return;
    }
    if (playbackOverlay) {
        playbackOverlay->hide();
    }
}

bool MainWindow::globalPointInVideo(const QPoint &globalPos) const {
    if (!videoWidget) {
        return false;
    }

    const QPoint localPos = videoWidget->mapFromGlobal(globalPos);
    return videoWidget->rect().contains(localPos);
}

bool MainWindow::isOverlayObject(QObject *object) const {
    auto *widget = qobject_cast<QWidget *>(object);
    return widget && playbackOverlay && (widget == playbackOverlay || playbackOverlay->isAncestorOf(widget));
}

QString MainWindow::formatPlaybackTime(qint64 milliseconds) const {
    if (milliseconds < 0) {
        return "--:--";
    }

    qint64 seconds = milliseconds / 1000;
    const qint64 hours = seconds / 3600;
    seconds %= 3600;
    const qint64 minutes = seconds / 60;
    seconds %= 60;

    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}
