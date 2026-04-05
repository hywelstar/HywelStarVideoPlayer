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
    // Default to directories under current executable path
    const QString appDir = QCoreApplication::applicationDirPath();
    setRecordingPath(appDir + "/video");
    setScreenshotPath(appDir + "/picture");
}

void RecordingManager::startRecording(const QString &format, const QString &quality, const QString &filepath) {
    currentFormat = format;
    currentQuality = quality;
    currentFilepath = filepath.isEmpty() ? generateFilename(format) : filepath;
    recordingTimer.start();
    emit recordingStarted(currentFilepath);
}

void RecordingManager::stopRecording() {
    recordingTimer.invalidate();
    emit recordingStopped(currentFilepath, 0);
}

QString RecordingManager::generateFilename(const QString &formatOverride) const {
    QDir().mkpath(recordingPath);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    QString extension = formatOverride.isEmpty() ? currentFormat : formatOverride;
    if (extension.isEmpty()) {
        extension = "mkv";
    }
    if (!extension.startsWith(".")) {
        extension = "." + extension;
    }
    return recordingPath + "/" + "recording_" + timestamp + extension;
}

QString RecordingManager::generateScreenshotFilename() {
    QDir().mkpath(screenshotPath);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz");
    return screenshotPath + "/" + "screenshot_" + timestamp + ".png";
}

void RecordingManager::setRecordingPath(const QString &path) {
    const QString appDir = QCoreApplication::applicationDirPath();
    recordingPath = path.trimmed().isEmpty() ? (appDir + "/video") : path;
    QDir dir(recordingPath);
    if (!dir.exists()) {
        dir.mkpath(recordingPath);
    }
}

void RecordingManager::setScreenshotPath(const QString &path) {
    const QString appDir = QCoreApplication::applicationDirPath();
    screenshotPath = path.trimmed().isEmpty() ? (appDir + "/picture") : path;
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

QString RecordingManager::getCurrentFilepath() const {
    return currentFilepath;
}
