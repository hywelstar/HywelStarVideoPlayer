/**
 * @file StatusBar.cpp
 * @brief Status bar implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "StatusBar.h"
#include "ThemeManager.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QDateTime>

StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
{
    connectionLabel = new QLabel("Disconnected");
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
    recordingLabel->setToolTip("Recording Status");
    addPermanentWidget(recordingLabel);

    timeLabel = new QLabel();
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    addPermanentWidget(timeLabel);

    applyTheme();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateTime);
    timer->start(1000);
}

void StatusBar::addSeparator() {
    QLabel *sep = new QLabel("|");
    sep->setStyleSheet(QString("color: %1; padding: 0 5px;").arg(ThemeManager::currentPalette().border));
    separatorLabels.append(sep);
    addWidget(sep);
}

void StatusBar::updateTime() {
    timeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void StatusBar::updateStreamInfo(int width, int height, int fps, int bitrate) {
    if (width > 0 && height > 0) {
        resolutionLabel->setText(QString("%1x%2").arg(width).arg(height));
        resolutionLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().success));
    } else {
        resolutionLabel->setText("--");
        resolutionLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().textSecondary));
    }

    if (fps > 0) {
        fpsLabel->setText(QString("%1 fps").arg(fps));
        fpsLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().success));
    } else {
        fpsLabel->setText("-- fps");
        fpsLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().textSecondary));
    }

    if (bitrate > 0) {
        bitrateLabel->setText(QString("%1 Mbps").arg(bitrate / 1000000.0, 0, 'f', 1));
        bitrateLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().success));
    } else {
        bitrateLabel->setText("-- Mbps");
        bitrateLabel->setStyleSheet(QString("color: %1;").arg(ThemeManager::currentPalette().textSecondary));
    }

}

void StatusBar::updateLatency(int latencyMs) {
    if (latencyMs >= 0) {
        delayLabel->setText(QString("%1 ms").arg(latencyMs));
        const ThemePalette palette = ThemeManager::currentPalette();
        delayLabel->setStyleSheet(QString("color: %1;").arg(latencyMs <= 200 ? palette.success : palette.warning));
    } else {
        delayLabel->setText("-- ms");
    }
}
void StatusBar::updateConnectionStatus(const QString &status) {
    connectionLabel->setText(status);
    const ThemePalette palette = ThemeManager::currentPalette();
    if (status.contains("Connected") || status.contains("Playing")) {
        connectionLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.success));
    } else if (status.contains("Error")) {
        connectionLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.danger));
    } else {
        connectionLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.textSecondary));
    }
}

void StatusBar::updateRecordingStatus(bool recording) {
    const ThemePalette palette = ThemeManager::currentPalette();
    if (recording) {
        recordingLabel->setText("REC ON");
        recordingLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.danger));
    } else {
        recordingLabel->setText("REC");
        recordingLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.textSecondary));
    }
}

void StatusBar::showError(const QString &error) {
    connectionLabel->setText("Error: " + error);
    connectionLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(ThemeManager::currentPalette().danger));
}

void StatusBar::applyTheme() {
    setStyleSheet(ThemeManager::statusBarStyle());
    const ThemePalette palette = ThemeManager::currentPalette();
    if (connectionLabel) {
        connectionLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.textSecondary));
    }
    if (recordingLabel) {
        recordingLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(palette.textSecondary));
    }
    if (timeLabel) {
        timeLabel->setStyleSheet(ThemeManager::timeLabelStyle());
    }
    for (QLabel *separator : separatorLabels) {
        if (separator) {
            separator->setStyleSheet(QString("color: %1; padding: 0 5px;").arg(palette.border));
        }
    }
}



