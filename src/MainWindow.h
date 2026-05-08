/**
 * @file MainWindow.h
 * @brief Main application window declaration
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui/ControlBar.h"
#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QMenuBar>
#include <QMenu>
#include <QPoint>
#include <memory>

class VideoDisplayWidget;
class ControlBar;
class StatusBar;
class QuickConnectBar;
class LocalFileListWidget;
class QLabel;
class QSlider;
class QSplitter;
class GStreamerEngine;
class RecordingManager;
class ConfigManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // QuickConnectBar slots
    void onPlayUri(const QString &uri);
    void onPlayLocalFile(const QString &filePath);
    void onShowSettings();
    void onShowAbout();

    // ControlBar slots
    void onPlayPause();
    void onStartRecording();
    void onStopRecording();
    void onScreenshot();
    void onToggleFullScreen();
    void onToggleGrid();
    void onVolumeChanged(int volume);
    void onSeekRequested(qint64 positionMs);
    void onStretchToggleRequested(bool stretch);
    void onPlaybackRateChanged(double rate);
    void onPlaybackEndModeChanged(PlaybackEndMode mode);

    // Engine slots
    void onStreamInfoChanged(int width, int height, int fps, int bitrate);
    void onPlayerStateChanged(int state);
    void onErrorOccurred(const QString &error);
    void onRecordingStatusChanged(bool recording, qint64 duration, qint64 fileSize);
    void onEndOfStream();

    // Timer slots
    void updateRecordingTime();
    void updatePlaybackPosition();

private:
    void setupUI();
    void setupMenuBar();
    void connectSignals();
    void loadSettings();
    void saveSettings();
    void setLocalFilePanelVisible(bool visible);
    void playLocalFilePath(const QString &filePath, bool restartPlayback);
    void setupPlaybackOverlay(QWidget *parent);
    void positionPlaybackOverlay();
    void showPlaybackOverlay();
    void hidePlaybackOverlay();
    bool globalPointInVideo(const QPoint &globalPos) const;
    bool isOverlayObject(QObject *object) const;
    QString formatPlaybackTime(qint64 milliseconds) const;

    std::unique_ptr<VideoDisplayWidget> videoWidget;
    std::unique_ptr<ControlBar> controlBar;
    std::unique_ptr<StatusBar> statusBar;
    std::unique_ptr<QuickConnectBar> quickConnectBar;
    std::unique_ptr<LocalFileListWidget> localFileList;
    std::unique_ptr<GStreamerEngine> gstreamerEngine;
    std::unique_ptr<RecordingManager> recordingManager;
    std::unique_ptr<ConfigManager> configManager;

    QSplitter *contentSplitter = nullptr;
    QWidget *playbackOverlay = nullptr;
    QLabel *playbackPositionLabel = nullptr;
    QSlider *playbackPositionSlider = nullptr;
    QLabel *playbackDurationLabel = nullptr;
    QTimer *recordingTimer;
    QTimer *playbackTimer;
    QTimer *overlayHideTimer;
    QTimer *clickTimer;
    QElapsedTimer recordingElapsed;
    bool isFullScreen = false;
    bool isGridVisible = false;
    bool localFilePanelVisible = false;
    PlaybackEndMode playbackEndMode = PlaybackEndMode::Stop;
    QString currentLocalFilePath;
    bool currentPlaybackIsLocal = false;
    bool localEndHandled = false;
    bool isUpdatingPlaybackOverlay = false;
    qint64 pendingSeekPositionMs = -1;
    int pendingSeekRefreshHoldTicks = 0;
    bool mousePressedInVideo = false;
    bool suppressClickToggle = false;
    QPoint videoMousePressGlobalPos;
};

#endif // MAINWINDOW_H
