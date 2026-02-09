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

class ControlBar : public QWidget {
    Q_OBJECT

public:
    ControlBar(QWidget *parent = nullptr);

    void setRecordingTime(qint64 milliseconds);
    void setRecordingActive(bool active);
    void setFullscreen(bool fullscreen);

signals:
    void playRequested();
    void pauseRequested();
    void stopRequested();
    void recordingRequested();
    void recordingStopRequested();
    void screenshotRequested();
    void fullscreenRequested();
    void volumeChanged(int volume);

private:
    void setupUI();
    void connectSignals();

    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *recordButton;
    QPushButton *screenshotButton;
    QPushButton *gridButton;
    QPushButton *fullscreenButton;
    QSlider *volumeSlider;
    QLabel *recordingTimeLabel;
    QLabel *volumeLabel;

    bool isRecording = false;
};

#endif // CONTROL_BAR_H
