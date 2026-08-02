/**
 * @file ThemeManager.cpp
 * @brief Centralized application theme implementation
 */

#include "ThemeManager.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

namespace {
ThemePalette lightPalette() {
    return {
        false,
        "#F5F6F8", "#F5F6F8", "#FFFFFF", "#F0F2F5", "#E7EAEE",
        "#DDE7F8", "#D7DCE3", "#2F343B", "#6B7280", "#8A94A3",
        "#7A97CC", "#6A87B8", "#4FA07A", "#D9A441", "#D96B6B",
        "rgba(0, 0, 0, 150)", "#D7DCE3", "#2F343B", "#DDE7F8", "#FFFFFF"
    };
}

ThemePalette darkPalette() {
    return {
        true,
        "#15181D", "#1E232A", "#252B33", "#303743", "#38414D",
        "#263A5E", "#3A424D", "#E8EAED", "#A8B0BA", "#7F8996",
        "#7AA2F7", "#648CD6", "#72C49A", "#E0B75A", "#EF7E7E",
        "rgba(0, 0, 0, 175)", "#3A424D", "#E8EAED", "#31476D", "#FFFFFF"
    };
}

QString commonControlStyle(const ThemePalette &p) {
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 0px 12px;
            min-width: 44px;
            min-height: 38px;
        }
        QPushButton:hover { background-color: %4; }
        QPushButton:pressed { background-color: %5; }
        QPushButton:checked {
            background-color: %6;
            border-color: %7;
            font-weight: 700;
        }
        QComboBox, QSpinBox, QLineEdit {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            min-height: 38px;
            padding: 0px 10px;
            selection-background-color: %6;
            selection-color: %8;
        }
        QComboBox {
            padding-right: 34px;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 30px;
            border-left: 1px solid %3;
            border-top-right-radius: 6px;
            border-bottom-right-radius: 6px;
            background-color: %4;
        }
        QComboBox::down-arrow {
            image: url(:/icons/combo_down);
            width: 13px;
            height: 13px;
        }
        QComboBox QAbstractItemView {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            selection-background-color: %6;
            selection-color: %8;
            outline: none;
        }
        QSpinBox {
            padding-right: 38px;
        }
        QSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 34px;
            border-left: 1px solid %3;
            border-bottom: 1px solid %3;
            border-top-right-radius: 6px;
            background-color: %6;
        }
        QSpinBox::up-button:hover {
            background-color: %4;
        }
        QSpinBox::up-arrow {
            image: url(:/icons/spin_plus);
            width: 12px;
            height: 12px;
        }
        QSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 34px;
            border-left: 1px solid %3;
            border-bottom-right-radius: 6px;
            background-color: %6;
        }
        QSpinBox::down-button:hover {
            background-color: %4;
        }
        QSpinBox::down-arrow {
            image: url(:/icons/spin_minus);
            width: 12px;
            height: 12px;
        }
        QLabel { color: %9; }
    )")
        .arg(p.controlBg, p.textPrimary, p.border, p.controlHover, p.controlPressed,
             p.checkedBg, p.accent, p.selectionText, p.textSecondary);
}
}

ThemeMode ThemeManager::modeFromString(const QString &value) {
    if (value.compare("dark", Qt::CaseInsensitive) == 0) {
        return ThemeMode::Dark;
    }
    if (value.compare("light", Qt::CaseInsensitive) == 0) {
        return ThemeMode::Light;
    }
    return ThemeMode::System;
}

QString ThemeManager::modeToString(ThemeMode mode) {
    switch (mode) {
    case ThemeMode::Light:
        return "light";
    case ThemeMode::Dark:
        return "dark";
    case ThemeMode::System:
    default:
        return "system";
    }
}

ThemeMode ThemeManager::resolveMode(ThemeMode mode) {
    if (mode != ThemeMode::System) {
        return mode;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        return ThemeMode::Dark;
    }
#endif
    return ThemeMode::Light;
}

ThemePalette ThemeManager::palette(ThemeMode mode) {
    return resolveMode(mode) == ThemeMode::Dark ? darkPalette() : lightPalette();
}

ThemePalette ThemeManager::currentPalette() {
    return palette(modeFromString(qApp->property("themeMode").toString()));
}

void ThemeManager::applyApplicationTheme(ThemeMode mode) {
    qApp->setProperty("themeMode", modeToString(mode));
    const ThemePalette p = palette(mode);

    QPalette appPalette;
    appPalette.setColor(QPalette::Window, p.windowBg);
    appPalette.setColor(QPalette::WindowText, p.textPrimary);
    appPalette.setColor(QPalette::Base, p.controlBg);
    appPalette.setColor(QPalette::AlternateBase, p.panelBg);
    appPalette.setColor(QPalette::Text, p.textPrimary);
    appPalette.setColor(QPalette::Button, p.controlBg);
    appPalette.setColor(QPalette::ButtonText, p.textPrimary);
    appPalette.setColor(QPalette::Highlight, p.selectionBg);
    appPalette.setColor(QPalette::HighlightedText, p.selectionText);
    QApplication::setPalette(appPalette);

    qApp->setStyleSheet(settingsDialogStyle());
}

QString ThemeManager::mainMenuStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QMenuBar {
            background-color: %1;
            color: %2;
            padding: 2px;
            border-bottom: 1px solid %3;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 5px 10px;
            border-radius: 4px;
        }
        QMenuBar::item:selected { background-color: %4; }
        QMenu {
            background-color: %5;
            color: %2;
            border: 1px solid %3;
        }
        QMenu::item:selected { background-color: %4; }
    )").arg(p.windowBg, p.textPrimary, p.border, p.controlHover, p.controlBg);
}

QString ThemeManager::splitterStyle() {
    const ThemePalette p = currentPalette();
    return QString("QSplitter::handle { background-color: %1; }").arg(p.border);
}

QString ThemeManager::controlBarStyle() {
    const ThemePalette p = currentPalette();
    return QString("QWidget { background-color: %1; }").arg(p.windowBg) + commonControlStyle(p) + QString(R"(
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: %1;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: %2;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover { background: %2; }
    )").arg(p.sliderGroove, p.sliderHandle);
}

QString ThemeManager::volumeSliderStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QSlider {
            background-color: %1;
            border-radius: 6px;
            border: 1px solid %2;
            padding: 4px;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: %3;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: %4;
            border: none;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
    )").arg(p.controlBg, p.border, p.sliderGroove, p.sliderHandle);
}

QString ThemeManager::quickConnectBarStyle() {
    const ThemePalette p = currentPalette();
    return QString("QWidget { background-color: %1; }").arg(p.windowBg) + commonControlStyle(p) + QString(R"(
        QPushButton#streamModeButton, QPushButton#localModeButton {
            min-width: 0px;
            max-width: 16777215px;
            min-height: 50px;
            max-height: 50px;
            padding: 0px 20px;
            font-size: 15px;
            font-weight: 700;
        }
        QLineEdit {
            min-height: 50px;
        }
    )");
}

QString ThemeManager::localFileListStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        LocalFileListWidget {
            background-color: %1;
            border-right: 1px solid %2;
        }
        QLabel { color: %3; }
        QPushButton {
            background-color: %4;
            color: %5;
            border: 1px solid %2;
            border-radius: 6px;
            min-height: 34px;
            padding: 0px 10px;
        }
        QPushButton:hover { background-color: %6; }
        QListWidget {
            background-color: %4;
            color: %5;
            border: 1px solid %2;
            border-radius: 6px;
            outline: none;
        }
        QListWidget::item {
            color: %5;
            padding: 7px 8px;
        }
        QListWidget::item:selected {
            background-color: %7;
            color: %8;
        }
    )").arg(p.windowBg, p.border, p.textSecondary, p.controlBg, p.textPrimary,
            p.controlHover, p.checkedBg, p.selectionText);
}

QString ThemeManager::statusBarStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QStatusBar {
            background-color: %1;
            border-top: 1px solid %2;
        }
        QStatusBar::item { border: none; }
        QLabel {
            color: %3;
            font-size: 12px;
            padding: 4px 8px;
        }
    )").arg(p.windowBg, p.border, p.textSecondary);
}

QString ThemeManager::timeLabelStyle() {
    const ThemePalette p = currentPalette();
    return QString("background-color: %1; color: %2; font-weight: bold; border-radius: 6px; border: 1px solid %3;")
        .arg(p.controlBg, p.textPrimary, p.border);
}

QString ThemeManager::settingsDialogStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QDialog, QWidget {
            background-color: %1;
            color: %2;
        }
        QTabWidget::pane {
            border: 1px solid %3;
            background-color: %1;
        }
        QTabBar::tab {
            background-color: %4;
            color: %5;
            border: 1px solid %3;
            padding: 7px 12px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background-color: %6;
            color: %2;
        }
        QGroupBox {
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            margin-top: 8px;
            padding-top: 12px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
    )").arg(p.windowBg, p.textPrimary, p.border, p.controlBg, p.textSecondary, p.panelBg)
        + commonControlStyle(p);
}

QString ThemeManager::primaryButtonStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: #FFFFFF;
            border: none;
            border-radius: 4px;
            padding: 0px;
        }
        QPushButton:hover { background-color: %2; }
    )").arg(p.accent, p.accentHover);
}

QString ThemeManager::playbackOverlayStyle() {
    const ThemePalette p = currentPalette();
    return QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 8px;
        }
        QLabel { color: #FFFFFF; font-weight: 600; }
        QSlider::groove:horizontal {
            height: 5px;
            background: rgba(255, 255, 255, 90);
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            width: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
    )").arg(p.overlayBg);
}
