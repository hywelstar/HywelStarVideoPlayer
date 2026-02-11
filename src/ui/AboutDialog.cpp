/**
 * @file AboutDialog.cpp
 * @brief About dialog implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About"));
    setFixedSize(400, 300);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // App icon
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(QIcon(":/icons/app_icon").pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(iconLabel);

    // App name
    QLabel *nameLabel = new QLabel("HywelStar Video Player");
    nameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
    nameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(nameLabel);

    // Version
    QLabel *versionLabel = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()));
    versionLabel->setStyleSheet("font-size: 14px; color: #666;");
    versionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(versionLabel);

    // Description
    QLabel *descLabel = new QLabel(tr("A cross-platform video player\nbased on GStreamer and Qt 6.10"));
    descLabel->setStyleSheet("font-size: 12px; color: #666;");
    descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descLabel);

    // Author info
    QLabel *authorLabel = new QLabel(tr("Author: hywelstar\nEmail: hywelstar@126.com"));
    authorLabel->setStyleSheet("font-size: 12px; color: #888;");
    authorLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(authorLabel);

    // Copyright
    QLabel *copyrightLabel = new QLabel("© 2026 HywelStar. All rights reserved.");
    copyrightLabel->setStyleSheet("font-size: 11px; color: #aaa;");
    copyrightLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(copyrightLabel);

    mainLayout->addStretch();

    // OK button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setFixedWidth(80);
    okButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #66BB6A;
        }
    )");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}
