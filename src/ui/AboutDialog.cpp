/**
 * @file AboutDialog.cpp
 * @brief About dialog implementation
 * @author hywelstar
 * @email hywelstar@163.com
 * @date 2026
 * @copyright MIT License
 */

#include "AboutDialog.h"
#include "ThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <Qt>
#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About"));
    setFixedSize(440, 360);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    const ThemePalette palette = ThemeManager::currentPalette();
    setStyleSheet(ThemeManager::settingsDialogStyle());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(32, 24, 32, 24);

    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(88, 88);
    QPixmap iconPixmap = QIcon(":/icons/app_icon").pixmap(QSize(80, 80));
    iconLabel->setPixmap(iconPixmap.scaled(78, 78, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);

    QLabel *nameLabel = new QLabel("HywelStar Video Player");
    nameLabel->setMinimumHeight(26);
    nameLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(palette.textPrimary));
    nameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(nameLabel);

    QLabel *versionLabel = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()));
    versionLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(palette.textSecondary));
    versionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(versionLabel);

    QLabel *descLabel = new QLabel(tr("A cross-platform video player\nbased on GStreamer and Qt 6.10"));
    descLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(palette.textSecondary));
    descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descLabel);

    QLabel *authorLabel = new QLabel(tr("Author: hywelstar\nEmail: hywelstar@163.com"));
    authorLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(palette.textMuted));
    authorLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(authorLabel);

    QLabel *githubLabel = new QLabel("<a href=\"https://github.com/hywelstar/HywelStarVideoPlayer\">GitHub: hywelstar/HywelStarVideoPlayer</a>");
    githubLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(palette.accent));
    githubLabel->setTextFormat(Qt::RichText);
    githubLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    githubLabel->setOpenExternalLinks(true);
    githubLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(githubLabel);

    QLabel *copyrightLabel = new QLabel("(C) 2026 HywelStar. All rights reserved.");
    copyrightLabel->setStyleSheet(QString("font-size: 11px; color: %1;").arg(palette.textMuted));
    copyrightLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(copyrightLabel);

    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setFixedSize(88, 38);
    okButton->setStyleSheet(ThemeManager::primaryButtonStyle());
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}




