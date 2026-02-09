/**
 * @file ConfigManager.h
 * @brief Application configuration management
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class ConfigManager {
public:
    ConfigManager();

    void loadConfig(const QString &filepath);
    void saveConfig(const QString &filepath);

    // General settings
    QString getLanguage() const;
    void setLanguage(const QString &lang);

    QString getTheme() const;
    void setTheme(const QString &theme);

    // Video settings
    int getBufferSize() const;
    void setBufferSize(int size);

    int getTimeout() const;
    void setTimeout(int timeout);

    bool getAutoReconnect() const;
    void setAutoReconnect(bool enabled);

    // Recording settings
    QString getRecordingFormat() const;
    void setRecordingFormat(const QString &format);

    QString getRecordingPath() const;
    void setRecordingPath(const QString &path);

    // Presets
    QJsonArray getPresets() const;
    void addPreset(const QJsonObject &preset);
    void removePreset(const QString &name);

private:
    QJsonObject configData;
    QString configFilepath;
};

#endif // CONFIG_MANAGER_H
