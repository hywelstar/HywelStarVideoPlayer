/**
 * @file QuickConnectBar.cpp
 * @brief Quick connection bar implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "QuickConnectBar.h"
#include "ThemeManager.h"
#include <QButtonGroup>
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
    layout->setContentsMargins(16, 11, 16, 11);
    layout->setSpacing(13);

    // App title label
    titleLabel = new QLabel("HywelStar Player");
    titleLabel->setVisible(false);
    layout->addWidget(titleLabel);

    modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);

    streamModeButton = new QPushButton(tr("Stream"));
    streamModeButton->setObjectName("streamModeButton");
    streamModeButton->setCheckable(true);
    streamModeButton->setChecked(true);
    streamModeButton->setFixedSize(176, 50);
    streamModeButton->setToolTip(tr("Play stream URLs"));
    modeGroup->addButton(streamModeButton, 0);
    layout->addWidget(streamModeButton);

    localModeButton = new QPushButton(tr("Local"));
    localModeButton->setObjectName("localModeButton");
    localModeButton->setCheckable(true);
    localModeButton->setFixedSize(158, 50);
    localModeButton->setToolTip(tr("Show local media files"));
    modeGroup->addButton(localModeButton, 1);
    layout->addWidget(localModeButton);

    // URI input
    uriInput = new QLineEdit();
    uriInput->setMinimumHeight(50);
    uriInput->setPlaceholderText(tr("Enter stream URL (RTSP, RTMP, SRT, HLS, DASH, HTTP) and press Enter"));
    layout->addWidget(uriInput, 1);

    // Settings button
    settingsButton = new QPushButton();
    settingsButton->setFixedSize(50, 50);
    settingsButton->setIcon(QIcon(":/icons/settings"));
    settingsButton->setIconSize(QSize(20, 20));
    settingsButton->setToolTip(tr("Settings"));
    layout->addWidget(settingsButton);

    applyTheme();
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
    connect(modeGroup, &QButtonGroup::idClicked, this, [this](int id) {
        const bool localMode = id == 1;
        setLocalMode(localMode);
        emit localModeChanged(localMode);
    });
}

QString QuickConnectBar::getStreamUri() const {
    if (isLocalMode()) {
        return lastStreamUri;
    }
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
        lastStreamUri = lastUri;
        uriInput->setText(lastStreamUri);
    }
}

void QuickConnectBar::setUri(const QString &uri) {
    if (uri.startsWith("file://", Qt::CaseInsensitive)) {
        if (isLocalMode()) {
            uriInput->clear();
        }
        return;
    }

    lastStreamUri = uri;
    uriInput->setText(uri);
}

void QuickConnectBar::setLocalMode(bool localMode) {
    if (localMode) {
        const QString currentText = uriInput->text().trimmed();
        if (!currentText.isEmpty() && !currentText.startsWith("file://", Qt::CaseInsensitive)) {
            lastStreamUri = currentText;
        }
    }

    localModeButton->setChecked(localMode);
    streamModeButton->setChecked(!localMode);

    if (localMode) {
        uriInput->clear();
        uriInput->setReadOnly(true);
        uriInput->setPlaceholderText(tr("Local mode: select files from the local file list"));
    } else {
        uriInput->setReadOnly(false);
        uriInput->setPlaceholderText(tr("Enter stream URL (RTSP, RTMP, SRT, HLS, DASH, HTTP) and press Enter"));
        if (uriInput->text().trimmed().isEmpty() && !lastStreamUri.isEmpty()) {
            uriInput->setText(lastStreamUri);
        }
    }
    uriInput->setVisible(true);
}

bool QuickConnectBar::isLocalMode() const {
    return localModeButton->isChecked();
}

void QuickConnectBar::applyTheme() {
    titleLabel->setStyleSheet(QString());
    uriInput->setStyleSheet(QString());
    settingsButton->setStyleSheet(QString());
    streamModeButton->setStyleSheet(QString());
    localModeButton->setStyleSheet(QString());
    setStyleSheet(ThemeManager::quickConnectBarStyle());
}
