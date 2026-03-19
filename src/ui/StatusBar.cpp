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
            background-color: #F5F6F8;
            border-top: 1px solid #D7DCE3;
        }
        QStatusBar::item {
            border: none;
        }
        QLabel {
            color: #6B7280;
            font-size: 12px;
            padding: 4px 8px;
        }
    )");

    connectionLabel = new QLabel("Disconnected");
    connectionLabel->setStyleSheet("color: #6B7280; font-weight: bold;");
    addWidget(connectionLabel);

    addSeparator();

    resolutionLabel = new QLabel("--");
    resolutionLabel->setToolTip("Resolution");
    addPermanentWidget(resolutionLabel);

    fpsLabel = new QLabel("-- fps");
    fpsLabel->setToolTip("Frame Rate");
    addPermanentWidget(fpsLabel);

    bitrateLabel = new QLabel("-- Mbps");
    bitrateLabel->setToolTip("Bitrate");
    addPermanentWidget(bitrateLabel);

    delayLabel = new QLabel("-- ms");
    delayLabel->setToolTip("Latency");
    addPermanentWidget(delayLabel);

    recordingLabel = new QLabel("REC");
    recordingLabel->setStyleSheet("color: #6B7280; font-weight: bold;");
    recordingLabel->setToolTip("Recording Status");
    addPermanentWidget(recordingLabel);

    timeLabel = new QLabel();
    timeLabel->setStyleSheet("background-color: #FFFFFF; color: #2F343B; font-weight: bold; border-radius: 6px; border: 1px solid #D7DCE3;");
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    addPermanentWidget(timeLabel);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateTime);
    timer->start(1000);
}

void StatusBar::addSeparator() {
    QLabel *sep = new QLabel("|");
    sep->setStyleSheet("color: #D7DCE3; padding: 0 5px;");
    addWidget(sep);
}

void StatusBar::updateTime() {
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void StatusBar::updateStreamInfo(int width, int height, int fps, int bitrate) {
    if (width > 0 && height > 0) {
        resolutionLabel->setText(QString("%1x%2").arg(width).arg(height));
        resolutionLabel->setStyleSheet("color: #4FA07A;");
    } else {
        resolutionLabel->setText("--");
        resolutionLabel->setStyleSheet("color: #6B7280;");
    }

    if (fps > 0) {
        fpsLabel->setText(QString("%1 fps").arg(fps));
        fpsLabel->setStyleSheet("color: #4FA07A;");
    } else {
        fpsLabel->setText("-- fps");
        fpsLabel->setStyleSheet("color: #6B7280;");
    }

    if (bitrate > 0) {
        bitrateLabel->setText(QString("%1 Mbps").arg(bitrate / 1000000.0, 0, 'f', 1));
        bitrateLabel->setStyleSheet("color: #4FA07A;");
    } else {
        bitrateLabel->setText("-- Mbps");
        bitrateLabel->setStyleSheet("color: #6B7280;");
    }

    delayLabel->setText("N/A");
    delayLabel->setStyleSheet("color: #6B7280;");
}

void StatusBar::updateConnectionStatus(const QString &status) {
    connectionLabel->setText(status);
    if (status.contains("Connected") || status.contains("Playing")) {
        connectionLabel->setStyleSheet("color: #4FA07A; font-weight: bold;");
    } else if (status.contains("Error")) {
        connectionLabel->setStyleSheet("color: #D96B6B; font-weight: bold;");
    } else {
        connectionLabel->setStyleSheet("color: #6B7280; font-weight: bold;");
    }
}

void StatusBar::updateRecordingStatus(bool recording) {
    if (recording) {
        recordingLabel->setText("REC ON");
        recordingLabel->setStyleSheet("color: #D96B6B; font-weight: bold;");
    } else {
        recordingLabel->setText("REC");
        recordingLabel->setStyleSheet("color: #555555; font-weight: bold;");
    }
}

void StatusBar::showError(const QString &error) {
    connectionLabel->setText("Error: " + error);
    connectionLabel->setStyleSheet("color: #D96B6B; font-weight: bold;");
}
