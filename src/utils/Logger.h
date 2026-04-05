/**
 * @file Logger.h
 * @brief Logging utility with file output support
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger &instance();

    void setLogLevel(LogLevel level);
    void setLogFile(const QString &filepath);

    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

private:
    Logger();
    ~Logger();

    void log(LogLevel level, const QString &message);

    LogLevel currentLevel = LogLevel::Info;
    QFile logFile;
    QTextStream logStream;
    QMutex logMutex;
    int bufferedLineCount = 0;
};

#endif // LOGGER_H
