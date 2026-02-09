/**
 * @file main.cpp
 * @brief Application entry point for HywelStar Video Player
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include <QApplication>
#include <QSysInfo>
#include <QDir>
#include <QFileInfo>
#include <cstdlib>
#include "MainWindow.h"
#include "utils/Logger.h"

void setupGStreamerEnv() {
    // Set GStreamer environment variables based on application directory
    const QString appDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_WIN
    // Try multiple possible plugin locations
    QStringList possiblePaths = {
        QDir(appDir).filePath("lib/gstreamer-1.0"),      // Same dir as exe: lib/gstreamer-1.0
        QDir(appDir).filePath("../lib/gstreamer-1.0"),   // Parent dir: ../lib/gstreamer-1.0
        QDir(appDir).filePath("gstreamer-1.0"),          // Same dir as exe: gstreamer-1.0
    };

    QString pluginDir;
    for (const QString &path : possiblePaths) {
        if (QFileInfo::exists(path)) {
            pluginDir = QDir(path).absolutePath();
            break;
        }
    }

    if (!pluginDir.isEmpty()) {
        qputenv("GST_PLUGIN_SYSTEM_PATH_1_0", pluginDir.toUtf8().constData());
        qputenv("GST_PLUGIN_SYSTEM_PATH", pluginDir.toUtf8().constData());
        qputenv("GST_PLUGIN_PATH_1_0", pluginDir.toUtf8().constData());
        qputenv("GST_PLUGIN_PATH", pluginDir.toUtf8().constData());
    }
#endif
}

int main(int argc, char *argv[]) {
    // Set GStreamer environment BEFORE QApplication
    // Note: QCoreApplication::applicationDirPath() requires QCoreApplication to exist
    // So we create a minimal app first, set env, then continue
    QApplication app(argc, argv);
    app.setApplicationName("HywelStarVideoPlayer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("HywelStar");

    // Setup GStreamer environment
    setupGStreamerEnv();

    // Setup logger
    Logger::instance().setLogLevel(LogLevel::Debug);
    Logger::instance().info("===========================================");
    Logger::instance().info("HywelStarVideoPlayer starting...");
    Logger::instance().info(QString("Version: %1").arg(app.applicationVersion()));
    Logger::instance().info(QString("Qt Version: %1").arg(qVersion()));
    Logger::instance().info(QString("OS: %1 %2").arg(QSysInfo::productType(), QSysInfo::productVersion()));
    Logger::instance().info(QString("Architecture: %1").arg(QSysInfo::currentCpuArchitecture()));
#ifdef Q_OS_WIN
    Logger::instance().info(QString("GST_PLUGIN_PATH: %1").arg(qgetenv("GST_PLUGIN_PATH").constData()));
#endif
    Logger::instance().info("===========================================");

    // Create and show main window
    MainWindow window;
    window.show();

    Logger::instance().info("Main window displayed, entering event loop");

    int result = app.exec();

    Logger::instance().info(QString("Application exiting with code: %1").arg(result));
    return result;
}
