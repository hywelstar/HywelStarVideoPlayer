/**
 * @file ConfigManager.cpp
 * @brief Application configuration management implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "ConfigManager.h"
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

ConfigManager::ConfigManager() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(configDir);
    }

    configFilepath = configDir + "/config.json";

    // Initialize default config
    configData["version"] = "1.0";
    configData["general"] = QJsonObject{
        {"language", "zh_CN"},
        {"theme", "dark"},
        {"logLevel", "info"}
    };
    configData["video"] = QJsonObject{
        {"bufferSize", 200},
        {"timeout", 5000},
        {"autoReconnect", true},
        {"reconnectAttempts", 3},
        {"decoderPreference", "hardware"}
    };
    configData["recording"] = QJsonObject{
        {"format", "mkv"},
        {"quality", "high"},
        {"savePath", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)},
        {"maxFileSize", 2048},
        {"autoRecord", false}
    };

    loadConfig(configFilepath);
}

void ConfigManager::loadConfig(const QString &filepath) {
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject()) {
            configData = doc.object();
        }
        file.close();
    }
}

void ConfigManager::saveConfig(const QString &filepath) {
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(configData);
        file.write(doc.toJson());
        file.close();
    }
}

QString ConfigManager::getLanguage() const {
    return configData["general"].toObject()["language"].toString("zh_CN");
}

void ConfigManager::setLanguage(const QString &lang) {
    QJsonObject general = configData["general"].toObject();
    general["language"] = lang;
    configData["general"] = general;
}

QString ConfigManager::getTheme() const {
    return configData["general"].toObject()["theme"].toString("dark");
}

void ConfigManager::setTheme(const QString &theme) {
    QJsonObject general = configData["general"].toObject();
    general["theme"] = theme;
    configData["general"] = general;
}

int ConfigManager::getBufferSize() const {
    return configData["video"].toObject()["bufferSize"].toInt(200);
}

void ConfigManager::setBufferSize(int size) {
    QJsonObject video = configData["video"].toObject();
    video["bufferSize"] = size;
    configData["video"] = video;
}

int ConfigManager::getTimeout() const {
    return configData["video"].toObject()["timeout"].toInt(5000);
}

void ConfigManager::setTimeout(int timeout) {
    QJsonObject video = configData["video"].toObject();
    video["timeout"] = timeout;
    configData["video"] = video;
}

bool ConfigManager::getAutoReconnect() const {
    return configData["video"].toObject()["autoReconnect"].toBool(true);
}

void ConfigManager::setAutoReconnect(bool enabled) {
    QJsonObject video = configData["video"].toObject();
    video["autoReconnect"] = enabled;
    configData["video"] = video;
}

QString ConfigManager::getRecordingFormat() const {
    return configData["recording"].toObject()["format"].toString("mkv");
}

void ConfigManager::setRecordingFormat(const QString &format) {
    QJsonObject recording = configData["recording"].toObject();
    recording["format"] = format;
    configData["recording"] = recording;
}

QString ConfigManager::getRecordingPath() const {
    return configData["recording"].toObject()["savePath"].toString();
}

void ConfigManager::setRecordingPath(const QString &path) {
    QJsonObject recording = configData["recording"].toObject();
    recording["savePath"] = path;
    configData["recording"] = recording;
}

QJsonArray ConfigManager::getPresets() const {
    return configData["presets"].toArray();
}

void ConfigManager::addPreset(const QJsonObject &preset) {
    QJsonArray presets = configData["presets"].toArray();
    presets.append(preset);
    configData["presets"] = presets;
}

void ConfigManager::removePreset(const QString &name) {
    QJsonArray presets = configData["presets"].toArray();
    QJsonArray newPresets;
    for (const auto &preset : presets) {
        if (preset.toObject()["name"].toString() != name) {
            newPresets.append(preset);
        }
    }
    configData["presets"] = newPresets;
}
