/**
 * @file RecordingManager.h
 * @brief Video recording and screenshot management
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef RECORDING_MANAGER_H
#define RECORDING_MANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QElapsedTimer>

class RecordingManager : public QObject {
    Q_OBJECT

public:
    RecordingManager(QObject *parent = nullptr);

    void startRecording(const QString &format, const QString &quality);
    void stopRecording();
    QString generateFilename();
    QString generateScreenshotFilename();
    void setRecordingPath(const QString &path);
    void setScreenshotPath(const QString &path);
    void setMaxFileSize(qint64 sizeInMB);

    QString getRecordingPath() const;
    QString getScreenshotPath() const;
    qint64 getMaxFileSize() const;

signals:
    void recordingStarted(const QString &filepath);
    void recordingStopped(const QString &filepath, qint64 fileSize);
    void fileSizeUpdated(qint64 currentSize);

private:
    QString recordingPath;
    QString screenshotPath;
    qint64 maxFileSize = 2048; // 2GB default
    QString currentFormat = "mkv";
    QString currentQuality = "high";
    QElapsedTimer recordingTimer;
    QString currentFilepath;
};

#endif // RECORDING_MANAGER_H
