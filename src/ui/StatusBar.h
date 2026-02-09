/**
 * @file StatusBar.h
 * @brief Status bar showing stream info and connection status
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar {
    Q_OBJECT

public:
    StatusBar(QWidget *parent = nullptr);

    void updateStreamInfo(int width, int height, int fps, int bitrate);
    void updateConnectionStatus(const QString &status);
    void updateRecordingStatus(bool recording);
    void showError(const QString &error);

private slots:
    void updateTime();

private:
    void addSeparator();

    QLabel *connectionLabel;
    QLabel *resolutionLabel;
    QLabel *fpsLabel;
    QLabel *bitrateLabel;
    QLabel *delayLabel;
    QLabel *recordingLabel;
    QLabel *timeLabel;
};

#endif // STATUS_BAR_H
