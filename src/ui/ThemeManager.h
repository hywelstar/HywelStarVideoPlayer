/**
 * @file ThemeManager.h
 * @brief Centralized application theme colors and styles
 */

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>

enum class ThemeMode {
    System,
    Light,
    Dark
};

struct ThemePalette {
    bool dark = false;
    QString windowBg;
    QString panelBg;
    QString controlBg;
    QString controlHover;
    QString controlPressed;
    QString checkedBg;
    QString border;
    QString textPrimary;
    QString textSecondary;
    QString textMuted;
    QString accent;
    QString accentHover;
    QString success;
    QString warning;
    QString danger;
    QString overlayBg;
    QString sliderGroove;
    QString sliderHandle;
    QString selectionBg;
    QString selectionText;
};

class ThemeManager {
public:
    static ThemeMode modeFromString(const QString &value);
    static QString modeToString(ThemeMode mode);
    static ThemeMode resolveMode(ThemeMode mode);
    static ThemePalette palette(ThemeMode mode);
    static ThemePalette currentPalette();
    static void applyApplicationTheme(ThemeMode mode);

    static QString mainMenuStyle();
    static QString splitterStyle();
    static QString controlBarStyle();
    static QString volumeSliderStyle();
    static QString quickConnectBarStyle();
    static QString localFileListStyle();
    static QString statusBarStyle();
    static QString timeLabelStyle();
    static QString settingsDialogStyle();
    static QString primaryButtonStyle();
    static QString playbackOverlayStyle();
};

#endif // THEME_MANAGER_H
