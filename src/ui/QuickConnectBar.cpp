/**
 * @file QuickConnectBar.cpp
 * @brief Quick connection bar implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "QuickConnectBar.h"
#include <QHBoxLayout>
#include <QSettings>
#include <QIcon>
#include <QFont>

QuickConnectBar::QuickConnectBar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
    loadHistory();
}

void QuickConnectBar::setupUI() {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(8);

    // App title label
    titleLabel = new QLabel("Hywel Star Video Player");
    layout->addWidget(titleLabel);

    // URI input
    uriInput = new QLineEdit();
    uriInput->setPlaceholderText(tr("Enter stream URL (rtsp://, udp://, http://...)"));
    layout->addWidget(uriInput, 1);

    // Connect button
    connectButton = new QPushButton(tr("Connect"));
    connectButton->setMinimumWidth(80);
    layout->addWidget(connectButton);

    // Disconnect button
    disconnectButton = new QPushButton(tr("Disconnect"));
    disconnectButton->setMinimumWidth(80);
    disconnectButton->setEnabled(false);
    layout->addWidget(disconnectButton);

    // Settings button
    settingsButton = new QPushButton();
    settingsButton->setIcon(QIcon(":/icons/settings"));
    settingsButton->setIconSize(QSize(20, 20));
    settingsButton->setToolTip(tr("Settings"));
    layout->addWidget(settingsButton);

    // Set individual styles
    titleLabel->setStyleSheet("background-color: #ffffff; color: #000000; font-weight: bold; padding: 4px 10px; border-radius: 4px;");

    uriInput->setStyleSheet(R"(
        QLineEdit {
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QLineEdit:focus {
            border: 1px solid #5c6bc0;
        }
    )");

    connectButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #66BB6A;
        }
        QPushButton:pressed {
            background-color: #388E3C;
        }
        QPushButton:disabled {
            background-color: #a5d6a7;
            color: #666666;
        }
    )");

    disconnectButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            color: #000000;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #c0c0c0;
        }
        QPushButton:disabled {
            background-color: #888888;
            color: #555555;
        }
    )");

    settingsButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            color: #000000;
            border: none;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #c0c0c0;
        }
    )");

    // Set widget background
    setStyleSheet("QWidget { background-color: #1a1a2e; }");
}

void QuickConnectBar::connectSignals() {
    connect(connectButton, &QPushButton::clicked, this, [this]() {
        QString uri = uriInput->text();
        if (!uri.isEmpty()) {
            emit connectRequested(uri);
            addToHistory(uri);
            isConnected = true;
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
        }
    });

    connect(disconnectButton, &QPushButton::clicked, this, [this]() {
        emit disconnectRequested();
        isConnected = false;
        connectButton->setEnabled(true);
        disconnectButton->setEnabled(false);
    });

    connect(uriInput, &QLineEdit::returnPressed, connectButton, &QPushButton::click);

    connect(settingsButton, &QPushButton::clicked, this, &QuickConnectBar::settingsRequested);
}

QString QuickConnectBar::getStreamUri() const {
    return uriInput->text();
}

void QuickConnectBar::addToHistory(const QString &uri) {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    QStringList history = settings.value("uriHistory", QStringList()).toStringList();
    if (!history.contains(uri)) {
        history.prepend(uri);
        if (history.size() > 10) {
            history.removeLast();
        }
        settings.setValue("uriHistory", history);
    }
}

void QuickConnectBar::loadHistory() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    QStringList history = settings.value("uriHistory", QStringList()).toStringList();

    // Set default URI if no history
    if (history.isEmpty()) {
        uriInput->setText("rtsp://192.168.1.39:8554/video/1080P30.264");
    } else {
        // Load last used URI
        uriInput->setText(history.first());
    }
}

void QuickConnectBar::setUri(const QString &uri) {
    uriInput->setText(uri);
}
