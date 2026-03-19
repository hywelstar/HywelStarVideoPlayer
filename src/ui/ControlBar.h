/**
 * @file ControlBar.h
 * @brief Playback control bar with buttons and volume slider
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef CONTROL_BAR_H
#define CONTROL_BAR_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

class ControlBar : public QWidget {
    Q_OBJECT

public:
    ControlBar(QWidget *parent = nullptr);

    void setRecordingTime(qint64 milliseconds);
    void setRecordingActive(bool active);
    void setFullscreen(bool fullscreen);
    void setPlaybackState(PlaybackState state);
    void setVolume(int volume);
    int volume() const;
    void setGridActive(bool active);

signals:
    void playPauseRequested();
    void recordingRequested();
    void recordingStopRequested();
    void screenshotRequested();
    void fullscreenRequested();
    void gridToggleRequested();
    void volumeChanged(int volume);

private:
    void setupUI();
    void connectSignals();
    void updatePlayPauseButton();

    QPushButton *playPauseButton;
    QPushButton *recordButton;
    QPushButton *screenshotButton;
    QPushButton *gridButton;
    QPushButton *fullscreenButton;
    QSlider *volumeSlider;
    QLabel *recordingTimeLabel;
    QLabel *volumeLabel;

    bool isRecording = false;
    PlaybackState playbackState = PlaybackState::Stopped;
};

#endif // CONTROL_BAR_H
