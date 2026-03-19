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

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QMenuBar>
#include <QMenu>
#include <memory>

class VideoDisplayWidget;
class ControlBar;
class StatusBar;
class QuickConnectBar;
class GStreamerEngine;
class RecordingManager;
class ConfigManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // QuickConnectBar slots
    void onPlayUri(const QString &uri);
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

    // Engine slots
    void onStreamInfoChanged(int width, int height, int fps, int bitrate);
    void onPlayerStateChanged(int state);
    void onErrorOccurred(const QString &error);
    void onRecordingStatusChanged(bool recording, qint64 duration, qint64 fileSize);

    // Timer slots
    void updateRecordingTime();

private:
    void setupUI();
    void setupMenuBar();
    void connectSignals();
    void loadSettings();
    void saveSettings();

    std::unique_ptr<VideoDisplayWidget> videoWidget;
    std::unique_ptr<ControlBar> controlBar;
    std::unique_ptr<StatusBar> statusBar;
    std::unique_ptr<QuickConnectBar> quickConnectBar;
    std::unique_ptr<GStreamerEngine> gstreamerEngine;
    std::unique_ptr<RecordingManager> recordingManager;
    std::unique_ptr<ConfigManager> configManager;

    QTimer *recordingTimer;
    QElapsedTimer recordingElapsed;
    bool isFullScreen = false;
    bool isGridVisible = false;
};

#endif // MAINWINDOW_H
