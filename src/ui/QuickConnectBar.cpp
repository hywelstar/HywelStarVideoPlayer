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
    titleLabel = new QLabel("HywelStar Player");
    layout->addWidget(titleLabel);

    // URI input
    uriInput = new QLineEdit();
    uriInput->setPlaceholderText(tr("Enter stream URL (rtsp://, udp://, http://...) and press Enter"));
    layout->addWidget(uriInput, 1);

    // Settings button
    settingsButton = new QPushButton();
    settingsButton->setIcon(QIcon(":/icons/settings"));
    settingsButton->setIconSize(QSize(20, 20));
    settingsButton->setToolTip(tr("Settings"));
    layout->addWidget(settingsButton);

    // Styles
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

    settingsButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ffffff;
            color: #000000;
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
    )");

    setStyleSheet("QWidget { background-color: #1a1a2e; }");
}

void QuickConnectBar::connectSignals() {
    connect(uriInput, &QLineEdit::returnPressed, this, [this]() {
        QString uri = uriInput->text().trimmed();
        if (!uri.isEmpty()) {
            emit playRequested(uri);
            addToHistory(uri);
        }
    });

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
    QString lastUri = settings.value("lastUri", "").toString();
    if (!lastUri.isEmpty()) {
        uriInput->setText(lastUri);
    }
}

void QuickConnectBar::setUri(const QString &uri) {
    uriInput->setText(uri);
}
