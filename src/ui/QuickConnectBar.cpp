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
#include <QFileDialog>

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
    uriInput->setPlaceholderText(tr("Enter stream URL or file path..."));
    layout->addWidget(uriInput, 1);

    // Play button
    playButton = new QPushButton();
    playButton->setIcon(QIcon(":/icons/play"));
    playButton->setIconSize(QSize(20, 20));
    playButton->setToolTip(tr("Play (Enter)"));
    layout->addWidget(playButton);

    // Open file button
    openFileButton = new QPushButton();
    openFileButton->setIcon(QIcon(":/icons/upload"));
    openFileButton->setIconSize(QSize(20, 20));
    openFileButton->setToolTip(tr("Open File"));
    layout->addWidget(openFileButton);

    // Open folder button
    openFolderButton = new QPushButton();
    openFolderButton->setIcon(QIcon(":/icons/expand"));
    openFolderButton->setIconSize(QSize(20, 20));
    openFolderButton->setToolTip(tr("Open Folder"));
    layout->addWidget(openFolderButton);

    // Settings button
    settingsButton = new QPushButton();
    settingsButton->setIcon(QIcon(":/icons/settings"));
    settingsButton->setIconSize(QSize(20, 20));
    settingsButton->setToolTip(tr("Settings"));
    layout->addWidget(settingsButton);

    // About button
    aboutButton = new QPushButton();
    aboutButton->setIcon(QIcon(":/icons/app_icon"));
    aboutButton->setIconSize(QSize(20, 20));
    aboutButton->setToolTip(tr("About"));
    layout->addWidget(aboutButton);

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

    QString buttonStyle = R"(
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
    )";

    playButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px;
            min-width: 36px;
            min-height: 36px;
        }
        QPushButton:hover {
            background-color: #66BB6A;
        }
        QPushButton:pressed {
            background-color: #388E3C;
        }
    )");

    openFileButton->setStyleSheet(buttonStyle);
    openFolderButton->setStyleSheet(buttonStyle);
    settingsButton->setStyleSheet(buttonStyle);
    aboutButton->setStyleSheet(buttonStyle);

    setStyleSheet("QWidget { background-color: #1a1a2e; }");
}

void QuickConnectBar::connectSignals() {
    connect(playButton, &QPushButton::clicked, this, [this]() {
        QString uri = uriInput->text().trimmed();
        if (!uri.isEmpty()) {
            emit playRequested(uri);
            addToHistory(uri);
        }
    });

    connect(uriInput, &QLineEdit::returnPressed, playButton, &QPushButton::click);

    connect(openFileButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this,
            tr("Open Video File"),
            QString(),
            tr("Video Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.webm *.m4v *.ts *.m2ts);;All Files (*)"));
        if (!filePath.isEmpty()) {
            uriInput->setText(filePath);
            emit openFileRequested(filePath);
        }
    });

    connect(openFolderButton, &QPushButton::clicked, this, [this]() {
        QString folderPath = QFileDialog::getExistingDirectory(this,
            tr("Open Video Folder"),
            QString(),
            QFileDialog::ShowDirsOnly);
        if (!folderPath.isEmpty()) {
            emit openFolderRequested(folderPath);
        }
    });

    connect(settingsButton, &QPushButton::clicked, this, &QuickConnectBar::settingsRequested);
    connect(aboutButton, &QPushButton::clicked, this, &QuickConnectBar::aboutRequested);
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
