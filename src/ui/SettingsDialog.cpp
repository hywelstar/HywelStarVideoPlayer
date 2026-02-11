/**
 * @file SettingsDialog.cpp
 * @brief Settings dialog implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QTabWidget>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setFixedSize(500, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Tab widget
    QTabWidget *tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);

    // === Playback Tab ===
    QWidget *playbackTab = new QWidget();
    QVBoxLayout *playbackLayout = new QVBoxLayout(playbackTab);

    QGroupBox *playbackGroup = new QGroupBox(tr("Playback Settings"));
    QGridLayout *playbackGrid = new QGridLayout(playbackGroup);

    playbackGrid->addWidget(new QLabel(tr("Loop Mode:")), 0, 0);
    loopModeCombo = new QComboBox();
    loopModeCombo->addItem(tr("No Loop"), 0);
    loopModeCombo->addItem(tr("Loop One"), 1);
    loopModeCombo->addItem(tr("Loop All"), 2);
    playbackGrid->addWidget(loopModeCombo, 0, 1);

    playbackGrid->addWidget(new QLabel(tr("Auto-play next:")), 1, 0);
    autoPlayNextCheck = new QCheckBox();
    autoPlayNextCheck->setChecked(true);
    playbackGrid->addWidget(autoPlayNextCheck, 1, 1);

    playbackGrid->addWidget(new QLabel(tr("Buffer size (ms):")), 2, 0);
    bufferSizeSpinBox = new QSpinBox();
    bufferSizeSpinBox->setRange(50, 5000);
    bufferSizeSpinBox->setValue(100);
    bufferSizeSpinBox->setSuffix(" ms");
    playbackGrid->addWidget(bufferSizeSpinBox, 2, 1);

    playbackLayout->addWidget(playbackGroup);
    playbackLayout->addStretch();
    tabWidget->addTab(playbackTab, tr("Playback"));

    // === Recording Tab ===
    QWidget *recordingTab = new QWidget();
    QVBoxLayout *recordingLayout = new QVBoxLayout(recordingTab);

    QGroupBox *recordingGroup = new QGroupBox(tr("Recording Settings"));
    QGridLayout *recordingGrid = new QGridLayout(recordingGroup);

    recordingGrid->addWidget(new QLabel(tr("Format:")), 0, 0);
    recordingFormatCombo = new QComboBox();
    recordingFormatCombo->addItem("MKV", "mkv");
    recordingFormatCombo->addItem("MP4", "mp4");
    recordingFormatCombo->addItem("AVI", "avi");
    recordingGrid->addWidget(recordingFormatCombo, 0, 1, 1, 2);

    recordingGrid->addWidget(new QLabel(tr("Recording Path:")), 1, 0);
    recordingPathEdit = new QLineEdit();
    recordingGrid->addWidget(recordingPathEdit, 1, 1);
    QPushButton *browseRecordingBtn = new QPushButton(tr("Browse"));
    connect(browseRecordingBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Recording Directory"),
                                                        recordingPathEdit->text());
        if (!dir.isEmpty()) {
            recordingPathEdit->setText(dir);
        }
    });
    recordingGrid->addWidget(browseRecordingBtn, 1, 2);

    recordingGrid->addWidget(new QLabel(tr("Screenshot Path:")), 2, 0);
    screenshotPathEdit = new QLineEdit();
    recordingGrid->addWidget(screenshotPathEdit, 2, 1);
    QPushButton *browseScreenshotBtn = new QPushButton(tr("Browse"));
    connect(browseScreenshotBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Screenshot Directory"),
                                                        screenshotPathEdit->text());
        if (!dir.isEmpty()) {
            screenshotPathEdit->setText(dir);
        }
    });
    recordingGrid->addWidget(browseScreenshotBtn, 2, 2);

    recordingLayout->addWidget(recordingGroup);
    recordingLayout->addStretch();
    tabWidget->addTab(recordingTab, tr("Recording"));

    // === Buttons ===
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setFixedWidth(80);
    okButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #66BB6A;
        }
    )");
    connect(okButton, &QPushButton::clicked, this, [this]() {
        saveSettings();
        accept();
    });
    buttonLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setFixedWidth(80);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    QString appDir = QCoreApplication::applicationDirPath();

    loopModeCombo->setCurrentIndex(settings.value("loopMode", 0).toInt());
    autoPlayNextCheck->setChecked(settings.value("autoPlayNext", true).toBool());
    bufferSizeSpinBox->setValue(settings.value("bufferSize", 100).toInt());

    recordingFormatCombo->setCurrentText(settings.value("recordingFormat", "MKV").toString());
    recordingPathEdit->setText(settings.value("recordingPath", appDir + "/video").toString());
    screenshotPathEdit->setText(settings.value("screenshotPath", appDir + "/picture").toString());
}

void SettingsDialog::saveSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");

    settings.setValue("loopMode", loopModeCombo->currentIndex());
    settings.setValue("autoPlayNext", autoPlayNextCheck->isChecked());
    settings.setValue("bufferSize", bufferSizeSpinBox->value());

    settings.setValue("recordingFormat", recordingFormatCombo->currentText());
    settings.setValue("recordingPath", recordingPathEdit->text());
    settings.setValue("screenshotPath", screenshotPathEdit->text());
}

QString SettingsDialog::getRecordingPath() const {
    return recordingPathEdit->text();
}

QString SettingsDialog::getScreenshotPath() const {
    return screenshotPathEdit->text();
}

QString SettingsDialog::getRecordingFormat() const {
    return recordingFormatCombo->currentData().toString();
}

int SettingsDialog::getBufferSize() const {
    return bufferSizeSpinBox->value();
}

bool SettingsDialog::getAutoPlayNext() const {
    return autoPlayNextCheck->isChecked();
}

int SettingsDialog::getLoopMode() const {
    return loopModeCombo->currentData().toInt();
}

void SettingsDialog::setRecordingPath(const QString &path) {
    recordingPathEdit->setText(path);
}

void SettingsDialog::setScreenshotPath(const QString &path) {
    screenshotPathEdit->setText(path);
}

void SettingsDialog::setRecordingFormat(const QString &format) {
    int index = recordingFormatCombo->findData(format);
    if (index >= 0) {
        recordingFormatCombo->setCurrentIndex(index);
    }
}

void SettingsDialog::setBufferSize(int size) {
    bufferSizeSpinBox->setValue(size);
}

void SettingsDialog::setAutoPlayNext(bool enabled) {
    autoPlayNextCheck->setChecked(enabled);
}

void SettingsDialog::setLoopMode(int mode) {
    loopModeCombo->setCurrentIndex(mode);
}
