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
    void onConnect(const QString &uri);
    void onDisconnect();
    void onPlay();
    void onPause();
    void onStop();
    void onStartRecording();
    void onStopRecording();
    void onScreenshot();
    void onToggleFullScreen();
    void onToggleGrid();
    void onVolumeChanged(int volume);
    void onStreamInfoChanged(int width, int height, int fps, int bitrate);
    void onPlayerStateChanged(int state);
    void onErrorOccurred(const QString &error);
    void updateRecordingTime();

private:
    void setupUI();
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
};

#endif // MAINWINDOW_H
