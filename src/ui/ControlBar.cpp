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
#include <QtGlobal>

ControlBar::ControlBar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
    setStyleSheet(R"(
        QWidget {
            background-color: #F5F6F8;
        }
        QPushButton {
            background-color: #FFFFFF;
            color: #2F343B;
            border: 1px solid #D7DCE3;
            border-radius: 6px;
            padding: 8px;
            min-width: 36px;
            min-height: 36px;
        }
        QPushButton:hover {
            background-color: #F0F2F5;
        }
        QPushButton:pressed {
            background-color: #E7EAEE;
        }
        QPushButton:checked {
            background-color: #DDE7F8;
            border-color: #9FB5DE;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #D7DCE3;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #7A97CC;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: #6A87B8;
        }
        QLabel {
            background: transparent;
            color: #6B7280;
        }
    )");
}

void ControlBar::setupUI() {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 8, 14, 8);
    layout->setSpacing(10);

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
    recordingTimeLabel->setStyleSheet("color: #D96B6B; font-weight: bold;");
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
    gridButton->setCheckable(true);
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
    volumeLabel->setStyleSheet("background-color: #FFFFFF; padding: 6px; border-radius: 6px; border: 1px solid #D7DCE3;");
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
            background-color: #FFFFFF;
            border-radius: 6px;
            border: 1px solid #D7DCE3;
            padding: 4px;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #D7DCE3;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #7A97CC;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: #6A87B8;
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
        recordButton->setStyleSheet("QPushButton { background-color: #F2DCDC; border: 1px solid #D9A9A9; color: #8F4A4A; } QPushButton:hover { background-color: #EDD2D2; } QPushButton:pressed { background-color: #E3C2C2; }");
        recordButton->setToolTip(tr("Stop Recording (R)"));
    } else {
        recordButton->setIcon(QIcon(":/icons/record"));
        recordButton->setStyleSheet("QPushButton { background-color: #FFFFFF; color: #2F343B; border: 1px solid #D7DCE3; border-radius: 6px; } QPushButton:hover { background-color: #F0F2F5; } QPushButton:pressed { background-color: #E7EAEE; }");
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

void ControlBar::setVolume(int volume) {
    volumeSlider->setValue(qBound(0, volume, 100));
}

int ControlBar::volume() const {
    return volumeSlider->value();
}

void ControlBar::setGridActive(bool active) {
    gridButton->setChecked(active);
}
