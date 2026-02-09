/**
 * @file StatusBar.cpp
 * @brief Status bar implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "StatusBar.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QDateTime>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
{
    setStyleSheet(R"(
        QStatusBar {
            background-color: #ffffff;
            border-top: 1px solid #2d2d4a;
        }
        QStatusBar::item {
            border: none;
        }
        QLabel {
            color: #888888;
            font-size: 12px;
            padding: 4px 8px;
        }
    )");

    // Connection status
    connectionLabel = new QLabel("Disconnected");
    connectionLabel->setStyleSheet("color: #888888; font-weight: bold;");
    addWidget(connectionLabel);

    // Separator
    addSeparator();

    // Resolution
    resolutionLabel = new QLabel("--");
    resolutionLabel->setToolTip("Resolution");
    addPermanentWidget(resolutionLabel);

    // FPS
    fpsLabel = new QLabel("-- fps");
    fpsLabel->setToolTip("Frame Rate");
    addPermanentWidget(fpsLabel);

    // Bitrate
    bitrateLabel = new QLabel("-- Mbps");
    bitrateLabel->setToolTip("Bitrate");
    addPermanentWidget(bitrateLabel);

    // Delay
    delayLabel = new QLabel("-- ms");
    delayLabel->setToolTip("Latency");
    addPermanentWidget(delayLabel);

    // Recording status
    recordingLabel = new QLabel("REC");
    recordingLabel->setStyleSheet("color: #000000; font-weight: bold;");
    recordingLabel->setToolTip("Recording Status");
    addPermanentWidget(recordingLabel);

    // Time
    timeLabel = new QLabel();
    timeLabel->setStyleSheet("background-color: #ffffff; color: #000000; font-weight: bold; border-radius: 4px;");
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    addPermanentWidget(timeLabel);

    // Update time every second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateTime);
    timer->start(1000);
}

void StatusBar::addSeparator() {
    QLabel *sep = new QLabel("|");
    sep->setStyleSheet("color: #3d3d5c; padding: 0 5px;");
    addWidget(sep);
}

void StatusBar::updateTime() {
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void StatusBar::updateStreamInfo(int width, int height, int fps, int bitrate) {
    if (width > 0 && height > 0) {
        resolutionLabel->setText(QString("%1x%2").arg(width).arg(height));
        resolutionLabel->setStyleSheet("color: #4CAF50;");
    } else {
        resolutionLabel->setText("--");
        resolutionLabel->setStyleSheet("color: #888888;");
    }

    if (fps > 0) {
        fpsLabel->setText(QString("%1 fps").arg(fps));
        fpsLabel->setStyleSheet("color: #4CAF50;");
    } else {
        fpsLabel->setText("-- fps");
        fpsLabel->setStyleSheet("color: #888888;");
    }

    if (bitrate > 0) {
        bitrateLabel->setText(QString("%1 Mbps").arg(bitrate / 1000.0, 0, 'f', 1));
        bitrateLabel->setStyleSheet("color: #4CAF50;");
    } else {
        bitrateLabel->setText("-- Mbps");
        bitrateLabel->setStyleSheet("color: #888888;");
    }
}

void StatusBar::updateConnectionStatus(const QString &status) {
    connectionLabel->setText(status);
    if (status.contains("Connected") || status.contains("Playing")) {
        connectionLabel->setStyleSheet("color: #4CAF50; font-weight: bold;");
    } else if (status.contains("Error")) {
        connectionLabel->setStyleSheet("color: #F44336; font-weight: bold;");
    } else {
        connectionLabel->setStyleSheet("color: #888888; font-weight: bold;");
    }
}

void StatusBar::updateRecordingStatus(bool recording) {
    if (recording) {
        recordingLabel->setText("● REC");
        recordingLabel->setStyleSheet("color: #F44336; font-weight: bold;");
    } else {
        recordingLabel->setText("REC");
        recordingLabel->setStyleSheet("color: #555555; font-weight: bold;");
    }
}

void StatusBar::showError(const QString &error) {
    connectionLabel->setText("Error: " + error);
    connectionLabel->setStyleSheet("color: #F44336; font-weight: bold;");
}
