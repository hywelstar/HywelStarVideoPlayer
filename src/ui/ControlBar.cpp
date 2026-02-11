/**
 * @file ControlBar.cpp
 * @brief Playback control bar implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "ControlBar.h"
#include <QHBoxLayout>
#include <QStyle>
#include <QIcon>

ControlBar::ControlBar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
    setStyleSheet(R"(
        QWidget {
            background-color: #1a1a2e;
        }
        QPushButton {
            background-color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px;
            min-width: 36px;
            min-height: 36px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #c0c0c0;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #000000;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #000000;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: #333333;
        }
        QLabel {
            background: #ffffff;
            color: #000000;
        }
    )");
}

void ControlBar::setupUI() {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(8);

    // Play/Pause button (combined)
    playPauseButton = new QPushButton();
    playPauseButton->setIcon(QIcon(":/icons/play"));
    playPauseButton->setIconSize(QSize(24, 24));
    playPauseButton->setToolTip(tr("Play/Pause (Space)"));
    layout->addWidget(playPauseButton);

    layout->addSpacing(15);

    // Recording button
    recordButton = new QPushButton();
    recordButton->setIcon(QIcon(":/icons/record"));
    recordButton->setIconSize(QSize(24, 24));
    recordButton->setToolTip(tr("Record (R)"));
    layout->addWidget(recordButton);

    // Recording time label
    recordingTimeLabel = new QLabel("00:00");
    recordingTimeLabel->setStyleSheet("color: #F44336; font-weight: bold;");
    recordingTimeLabel->setMinimumWidth(50);
    layout->addWidget(recordingTimeLabel);

    layout->addSpacing(15);

    // Screenshot button
    screenshotButton = new QPushButton();
    screenshotButton->setIcon(QIcon(":/icons/screenshot"));
    screenshotButton->setIconSize(QSize(24, 24));
    screenshotButton->setToolTip(tr("Screenshot (S)"));
    layout->addWidget(screenshotButton);

    // Grid button
    gridButton = new QPushButton();
    gridButton->setIcon(QIcon(":/icons/grid"));
    gridButton->setIconSize(QSize(24, 24));
    gridButton->setToolTip(tr("Toggle Grid (G)"));
    layout->addWidget(gridButton);

    // Fullscreen button
    fullscreenButton = new QPushButton();
    fullscreenButton->setIcon(QIcon(":/icons/fullscreen"));
    fullscreenButton->setIconSize(QSize(24, 24));
    fullscreenButton->setToolTip(tr("Fullscreen (F)"));
    layout->addWidget(fullscreenButton);

    layout->addStretch();

    // Volume icon
    volumeLabel = new QLabel();
    volumeLabel->setPixmap(QIcon(":/icons/volume").pixmap(20, 20));
    volumeLabel->setStyleSheet("background-color: #a2a1a1; padding: 4px; border-radius: 4px;");
    layout->addWidget(volumeLabel);

    // Volume slider
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(120);
    volumeSlider->setToolTip(tr("Volume"));
    volumeSlider->setStyleSheet(R"(
        QSlider {
            background-color: #ffffff;
            border-radius: 4px;
            padding: 4px;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #000000;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #000000;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: #333333;
        }
    )");
    layout->addWidget(volumeSlider);
}

void ControlBar::connectSignals() {
    connect(playPauseButton, &QPushButton::clicked, this, &ControlBar::playPauseRequested);
    connect(recordButton, &QPushButton::clicked, this, [this]() {
        if (!isRecording) {
            emit recordingRequested();
        } else {
            emit recordingStopRequested();
        }
    });
    connect(screenshotButton, &QPushButton::clicked, this, &ControlBar::screenshotRequested);
    connect(gridButton, &QPushButton::clicked, this, &ControlBar::gridToggleRequested);
    connect(fullscreenButton, &QPushButton::clicked, this, &ControlBar::fullscreenRequested);
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        if (value == 0) {
            volumeLabel->setPixmap(QIcon(":/icons/volume_mute").pixmap(20, 20));
        } else {
            volumeLabel->setPixmap(QIcon(":/icons/volume").pixmap(20, 20));
        }
        emit volumeChanged(value);
    });
}

void ControlBar::updatePlayPauseButton() {
    switch (playbackState) {
    case PlaybackState::Stopped:
    case PlaybackState::Paused:
        playPauseButton->setIcon(QIcon(":/icons/play"));
        playPauseButton->setToolTip(tr("Play (Space)"));
        break;
    case PlaybackState::Playing:
        playPauseButton->setIcon(QIcon(":/icons/pause"));
        playPauseButton->setToolTip(tr("Pause (Space)"));
        break;
    }
}

void ControlBar::setPlaybackState(PlaybackState state) {
    playbackState = state;
    updatePlayPauseButton();
}

void ControlBar::setRecordingTime(qint64 milliseconds) {
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    recordingTimeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
}

void ControlBar::setRecordingActive(bool active) {
    isRecording = active;
    if (active) {
        recordButton->setIcon(QIcon(":/icons/stop"));
        recordButton->setStyleSheet("QPushButton { background-color: #F44336; } QPushButton:hover { background-color: #E53935; } QPushButton:pressed { background-color: #D32F2F; }");
        recordButton->setToolTip(tr("Stop Recording (R)"));
    } else {
        recordButton->setIcon(QIcon(":/icons/record"));
        recordButton->setStyleSheet("QPushButton { background-color: #ffffff; } QPushButton:hover { background-color: #e0e0e0; } QPushButton:pressed { background-color: #c0c0c0; }");
        recordButton->setToolTip(tr("Record (R)"));
        recordingTimeLabel->setText("00:00");
    }
}

void ControlBar::setFullscreen(bool fullscreen) {
    if (fullscreen) {
        fullscreenButton->setIcon(QIcon(":/icons/fullscreen_exit"));
        fullscreenButton->setToolTip(tr("Exit Fullscreen (F)"));
    } else {
        fullscreenButton->setIcon(QIcon(":/icons/fullscreen"));
        fullscreenButton->setToolTip(tr("Fullscreen (F)"));
    }
}
