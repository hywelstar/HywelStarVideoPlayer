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

class QComboBox;

enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

enum class PlaybackEndMode {
    Stop,
    RepeatOne,
    RepeatAll
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
    void setStretchActive(bool active);
    void setPlaybackRate(double rate);
    void setPlaybackEndMode(PlaybackEndMode mode);
    PlaybackEndMode playbackEndMode() const;
    void applyTheme();

signals:
    void playPauseRequested();
    void recordingRequested();
    void recordingStopRequested();
    void screenshotRequested();
    void fullscreenRequested();
    void gridToggleRequested();
    void volumeChanged(int volume);
    void stretchToggleRequested(bool stretch);
    void playbackRateChanged(double rate);
    void playbackEndModeChanged(PlaybackEndMode mode);

private:
    void setupUI();
    void connectSignals();
    void updatePlayPauseButton();

    QPushButton *playPauseButton;
    QPushButton *recordButton;
    QPushButton *screenshotButton;
    QPushButton *gridButton;
    QPushButton *stretchButton;
    QPushButton *fullscreenButton;
    QComboBox *endModeComboBox;
    QComboBox *speedComboBox;
    QSlider *volumeSlider;
    QLabel *recordingTimeLabel;
    QPushButton *muteButton;

    bool isRecording = false;
    PlaybackState playbackState = PlaybackState::Stopped;
    int lastNonZeroVolume = 50;
};

#endif // CONTROL_BAR_H
