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
    layout->setContentsMargins(14, 8, 14, 8);
    layout->setSpacing(10);

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

    titleLabel->setStyleSheet(R"(
        background-color: #FFFFFF;
        color: #2F343B;
        font-weight: 700;
        padding: 6px 10px;
        border-radius: 6px;
        border: 1px solid #D7DCE3;
    )");

    uriInput->setStyleSheet(R"(
        QLineEdit {
            background-color: #FFFFFF;
            color: #2F343B;
            border: 1px solid #D7DCE3;
            border-radius: 6px;
            padding: 8px 12px;
            selection-background-color: #DDE7F8;
            selection-color: #ffffff;
        }
        QLineEdit:focus {
            border: 1px solid #7A97CC;
        }
    )");

    settingsButton->setStyleSheet(R"(
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
    )");

    setStyleSheet("QWidget { background-color: #F5F6F8; }");
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
