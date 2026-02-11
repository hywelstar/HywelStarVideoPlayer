/**
 * @file SettingsDialog.h
 * @brief Settings dialog for application configuration
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Getters
    QString getRecordingPath() const;
    QString getScreenshotPath() const;
    QString getRecordingFormat() const;
    int getBufferSize() const;
    bool getAutoPlayNext() const;
    int getLoopMode() const;

    // Setters
    void setRecordingPath(const QString &path);
    void setScreenshotPath(const QString &path);
    void setRecordingFormat(const QString &format);
    void setBufferSize(int size);
    void setAutoPlayNext(bool enabled);
    void setLoopMode(int mode);

private:
    void setupUI();
    void loadSettings();
    void saveSettings();

    // Playback settings
    QComboBox *loopModeCombo;
    QCheckBox *autoPlayNextCheck;
    QSpinBox *bufferSizeSpinBox;

    // Recording settings
    QComboBox *recordingFormatCombo;
    QLineEdit *recordingPathEdit;
    QLineEdit *screenshotPathEdit;
};

#endif // SETTINGS_DIALOG_H
