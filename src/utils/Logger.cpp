/**
 * @file Logger.cpp
 * @brief Logging utility implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "Logger.h"
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>
#include <QMutexLocker>
#include <iostream>

Logger::Logger() {
    // Get the directory where the executable is located
    QString logDir = QCoreApplication::applicationDirPath();
    QString logPath = logDir + "/app.log";

    logFile.setFileName(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    }
}

Logger::~Logger() {
    QMutexLocker locker(&logMutex);
    if (logFile.isOpen()) {
        if (bufferedLineCount > 0) {
            logStream.flush();
            bufferedLineCount = 0;
        }
        logFile.close();
    }
}

Logger &Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::setLogFile(const QString &filepath) {
    QMutexLocker locker(&logMutex);
    if (logFile.isOpen()) {
        if (bufferedLineCount > 0) {
            logStream.flush();
            bufferedLineCount = 0;
        }
        logFile.close();
    }
    logFile.setFileName(filepath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    }
}

void Logger::debug(const QString &message) {
    if (currentLevel <= LogLevel::Debug) {
        log(LogLevel::Debug, message);
    }
}

void Logger::info(const QString &message) {
    if (currentLevel <= LogLevel::Info) {
        log(LogLevel::Info, message);
    }
}

void Logger::warning(const QString &message) {
    if (currentLevel <= LogLevel::Warning) {
        log(LogLevel::Warning, message);
    }
}

void Logger::error(const QString &message) {
    if (currentLevel <= LogLevel::Error) {
        log(LogLevel::Error, message);
    }
}

void Logger::log(LogLevel level, const QString &message) {
    QString levelStr;
    switch (level) {
    case LogLevel::Debug:
        levelStr = "DEBUG";
        break;
    case LogLevel::Info:
        levelStr = "INFO";
        break;
    case LogLevel::Warning:
        levelStr = "WARNING";
        break;
    case LogLevel::Error:
        levelStr = "ERROR";
        break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("[%1] %2: %3").arg(timestamp, levelStr, message);

    std::cout << logMessage.toStdString() << '\n';

    QMutexLocker locker(&logMutex);
    if (logFile.isOpen()) {
        logStream << logMessage << "\n";
        bufferedLineCount += 1;
        if (level >= LogLevel::Error || bufferedLineCount >= 50) {
            logStream.flush();
            bufferedLineCount = 0;
        }
    }
}

