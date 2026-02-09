/**
 * @file RecordingManager.cpp
 * @brief Video recording management implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "RecordingManager.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

RecordingManager::RecordingManager(QObject *parent)
    : QObject(parent)
{
    // Use application directory for video and picture storage
    QString appDir = QCoreApplication::applicationDirPath();

    // Setup video recording path
    recordingPath = appDir + "/video";
    QDir videoDir(recordingPath);
    if (!videoDir.exists()) {
        videoDir.mkpath(recordingPath);
    }

    // Setup screenshot path
    screenshotPath = appDir + "/picture";
    QDir pictureDir(screenshotPath);
    if (!pictureDir.exists()) {
        pictureDir.mkpath(screenshotPath);
    }
}

void RecordingManager::startRecording(const QString &format, const QString &quality) {
    currentFormat = format;
    currentQuality = quality;
    currentFilepath = generateFilename();
    recordingTimer.start();
    emit recordingStarted(currentFilepath);
}

void RecordingManager::stopRecording() {
    recordingTimer.invalidate();
    emit recordingStopped(currentFilepath, 0);
}

QString RecordingManager::generateFilename() {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    QString extension = currentFormat.isEmpty() ? "mkv" : currentFormat;
    if (!extension.startsWith(".")) {
        extension = "." + extension;
    }
    return recordingPath + "/" + "recording_" + timestamp + extension;
}

QString RecordingManager::generateScreenshotFilename() {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz");
    return screenshotPath + "/" + "screenshot_" + timestamp + ".png";
}

void RecordingManager::setRecordingPath(const QString &path) {
    recordingPath = path;
    QDir dir(recordingPath);
    if (!dir.exists()) {
        dir.mkpath(recordingPath);
    }
}

void RecordingManager::setScreenshotPath(const QString &path) {
    screenshotPath = path;
    QDir dir(screenshotPath);
    if (!dir.exists()) {
        dir.mkpath(screenshotPath);
    }
}

void RecordingManager::setMaxFileSize(qint64 sizeInMB) {
    maxFileSize = sizeInMB;
}

QString RecordingManager::getRecordingPath() const {
    return recordingPath;
}

QString RecordingManager::getScreenshotPath() const {
    return screenshotPath;
}

qint64 RecordingManager::getMaxFileSize() const {
    return maxFileSize;
}
