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
#include "ui/SettingsDialog.h"
#include "ui/AboutDialog.h"
#include "core/GStreamerEngine.h"
#include "core/RecordingManager.h"
#include "core/ConfigManager.h"
#include "utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QImage>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoWidget(std::make_unique<VideoDisplayWidget>())
    , controlBar(std::make_unique<ControlBar>())
    , statusBar(std::make_unique<StatusBar>())
    , quickConnectBar(std::make_unique<QuickConnectBar>())
    , gstreamerEngine(std::make_unique<GStreamerEngine>())
    , recordingManager(std::make_unique<RecordingManager>())
    , configManager(std::make_unique<ConfigManager>())
    , recordingTimer(new QTimer(this))
{
    Logger::instance().info("MainWindow: Initializing main window...");
    setWindowTitle("HywelStar Video Player");
    setWindowIcon(QIcon(":/icons/app_icon"));
    resize(1280, 720);

    setupMenuBar();
    setupUI();
    connectSignals();
    loadSettings();

    // Setup recording timer
    connect(recordingTimer, &QTimer::timeout, this, &MainWindow::updateRecordingTime);

    Logger::instance().info("MainWindow: Initialization complete");
}

MainWindow::~MainWindow() {
    Logger::instance().info("MainWindow: Shutting down...");
    saveSettings();
}

void MainWindow::setupMenuBar() {
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(R"(
        QMenuBar {
            background-color: #1a1a2e;
            color: #ffffff;
            padding: 2px;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 10px;
        }
        QMenuBar::item:selected {
            background-color: #3d3d5c;
        }
        QMenu {
            background-color: #1a1a2e;
            color: #ffffff;
            border: 1px solid #3d3d5c;
        }
        QMenu::item {
            padding: 6px 20px;
        }
        QMenu::item:selected {
            background-color: #3d3d5c;
        }
    )");

    QMenu *helpMenu = menuBar->addMenu(tr("Help"));
    QAction *aboutAction = helpMenu->addAction(tr("About"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onShowAbout);

    setMenuBar(menuBar);
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #1a1a2e;");
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add quick connect bar
    mainLayout->addWidget(quickConnectBar.get());

    // Add video display widget
    mainLayout->addWidget(videoWidget.get(), 1);

    // Add control bar
    mainLayout->addWidget(controlBar.get());

    // Add status bar
    setStatusBar(statusBar.get());

    setCentralWidget(centralWidget);

    // Set window handle for GStreamer
    gstreamerEngine->setWindowHandle((WId)videoWidget->winId());
}

void MainWindow::connectSignals() {
    // Quick connect bar signals
    connect(quickConnectBar.get(), &QuickConnectBar::playRequested,
            this, &MainWindow::onPlayUri);
    connect(quickConnectBar.get(), &QuickConnectBar::settingsRequested,
            this, &MainWindow::onShowSettings);

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

    // Video display widget signals
    connect(videoWidget.get(), &VideoDisplayWidget::fullScreenRequested,
            this, &MainWindow::onToggleFullScreen);
    connect(videoWidget.get(), &VideoDisplayWidget::gridToggleRequested,
            this, &MainWindow::onToggleGrid);

    // GStreamer engine signals
    connect(gstreamerEngine.get(), QOverload<int, int, int, int>::of(&GStreamerEngine::streamInfoChanged),
            this, &MainWindow::onStreamInfoChanged);
    connect(gstreamerEngine.get(), QOverload<PlayerState>::of(&GStreamerEngine::stateChanged),
            this, [this](PlayerState state) { onPlayerStateChanged((int)state); });
    connect(gstreamerEngine.get(), &GStreamerEngine::errorOccurred,
            this, &MainWindow::onErrorOccurred);
}

void MainWindow::onPlayUri(const QString &uri) {
    Logger::instance().info(QString("MainWindow: Playing URI: %1").arg(uri));
    gstreamerEngine->play(uri);
    controlBar->setPlaybackState(PlaybackState::Playing);
}

void MainWindow::onShowSettings() {
    SettingsDialog dialog(this);
    dialog.setRecordingPath(recordingManager->getRecordingPath());
    dialog.setScreenshotPath(recordingManager->getScreenshotPath());

    if (dialog.exec() == QDialog::Accepted) {
        recordingManager->setRecordingPath(dialog.getRecordingPath());
        recordingManager->setScreenshotPath(dialog.getScreenshotPath());
        Logger::instance().info("MainWindow: Settings updated");
    }
}

void MainWindow::onShowAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onPlayPause() {
    if (gstreamerEngine->isPlaying()) {
        Logger::instance().info("MainWindow: Pause requested");
        gstreamerEngine->pause();
        controlBar->setPlaybackState(PlaybackState::Paused);
    } else {
        Logger::instance().info("MainWindow: Play requested");
        QString uri = quickConnectBar->getStreamUri();
        if (!uri.isEmpty()) {
            gstreamerEngine->play(uri);
            controlBar->setPlaybackState(PlaybackState::Playing);
        }
    }
}

void MainWindow::onStartRecording() {
    QString filepath = recordingManager->generateFilename();
    Logger::instance().info(QString("MainWindow: Starting recording to: %1").arg(filepath));
    gstreamerEngine->startRecording(filepath);
    recordingManager->startRecording("mkv", "high");
    controlBar->setRecordingActive(true);

    // Start recording timer
    recordingElapsed.start();
    recordingTimer->start(1000);
}

void MainWindow::onStopRecording() {
    Logger::instance().info("MainWindow: Stopping recording");
    gstreamerEngine->stopRecording();
    recordingManager->stopRecording();
    controlBar->setRecordingActive(false);

    // Stop recording timer
    recordingTimer->stop();
    controlBar->setRecordingTime(0);
}

void MainWindow::updateRecordingTime() {
    if (gstreamerEngine->isRecording()) {
        controlBar->setRecordingTime(recordingElapsed.elapsed());
    }
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
    if (isFullScreen) {
        Logger::instance().debug("MainWindow: Exiting fullscreen");
        quickConnectBar->show();
        controlBar->show();
        statusBar->show();
        showNormal();
        isFullScreen = false;
    } else {
        Logger::instance().debug("MainWindow: Entering fullscreen");
        quickConnectBar->hide();
        controlBar->hide();
        statusBar->hide();
        showFullScreen();
        isFullScreen = true;
    }
    controlBar->setFullscreen(isFullScreen);
}

void MainWindow::onToggleGrid() {
    videoWidget->toggleGrid();
}

void MainWindow::onVolumeChanged(int volume) {
    gstreamerEngine->setVolume(volume);
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

    // Last URI
    QString lastUri = settings.value("lastUri", "").toString();
    if (!lastUri.isEmpty()) {
        quickConnectBar->setUri(lastUri);
    }

    // Volume
    int volume = settings.value("volume", 50).toInt();
    gstreamerEngine->setVolume(volume);

    // Recording paths
    QString recordingPath = settings.value("recordingPath", "").toString();
    if (!recordingPath.isEmpty()) {
        recordingManager->setRecordingPath(recordingPath);
    }
    QString screenshotPath = settings.value("screenshotPath", "").toString();
    if (!screenshotPath.isEmpty()) {
        recordingManager->setScreenshotPath(screenshotPath);
    }

    Logger::instance().debug("MainWindow: Settings loaded");
}

void MainWindow::saveSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");

    // Window geometry
    settings.setValue("geometry", saveGeometry());

    // Last URI
    settings.setValue("lastUri", quickConnectBar->getStreamUri());

    // Recording paths
    settings.setValue("recordingPath", recordingManager->getRecordingPath());
    settings.setValue("screenshotPath", recordingManager->getScreenshotPath());

    Logger::instance().debug("MainWindow: Settings saved");
}
