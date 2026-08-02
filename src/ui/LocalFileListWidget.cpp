/**
 * @file LocalFileListWidget.cpp
 * @brief Local media file list widget implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "LocalFileListWidget.h"
#include "ThemeManager.h"
#include <QAbstractItemView>
#include <QColor>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

namespace {
constexpr int kMaxStoredFiles = 500;
constexpr int kPathRole = Qt::UserRole;
constexpr int kBaseNameRole = Qt::UserRole + 1;
}

LocalFileListWidget::LocalFileListWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
}

void LocalFileListWidget::setupUI() {
    setMinimumWidth(220);
    setMaximumWidth(380);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(8);

    titleLabel = new QLabel(tr("Local Files"));
    layout->addWidget(titleLabel);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(6);

    addFilesButton = new QPushButton(tr("Files"));
    addFilesButton->setToolTip(tr("Add media files"));
    buttonLayout->addWidget(addFilesButton);

    addFolderButton = new QPushButton(tr("Folder"));
    addFolderButton->setToolTip(tr("Add media files from folder"));
    buttonLayout->addWidget(addFolderButton);

    layout->addLayout(buttonLayout);

    fileList = new QListWidget();
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileList->setAlternatingRowColors(false);
    layout->addWidget(fileList, 1);

    emptyLabel = new QLabel(tr("Add files or folders to build a local playback list."));
    emptyLabel->setWordWrap(true);
    emptyLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(emptyLabel);

    auto *manageLayout = new QHBoxLayout();
    manageLayout->setSpacing(6);

    removeButton = new QPushButton(tr("Remove"));
    removeButton->setToolTip(tr("Remove selected files"));
    manageLayout->addWidget(removeButton);

    clearButton = new QPushButton(tr("Clear"));
    clearButton->setToolTip(tr("Clear file list"));
    manageLayout->addWidget(clearButton);

    layout->addLayout(manageLayout);
    applyTheme();
    updateEmptyState();
}

void LocalFileListWidget::connectSignals() {
    connect(addFilesButton, &QPushButton::clicked, this, &LocalFileListWidget::addFiles);
    connect(addFolderButton, &QPushButton::clicked, this, &LocalFileListWidget::addFolder);
    connect(removeButton, &QPushButton::clicked, this, &LocalFileListWidget::removeSelected);
    connect(clearButton, &QPushButton::clicked, this, &LocalFileListWidget::clearFiles);
    connect(fileList, &QListWidget::itemDoubleClicked, this, &LocalFileListWidget::playItem);
}

void LocalFileListWidget::addFiles() {
    const QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Add Media Files"),
        QString(),
        tr("Media Files (%1);;All Files (*)").arg(supportedNameFilters().join(' ')));
    addFilePaths(files);
}

void LocalFileListWidget::addFolder() {
    const QString folder = QFileDialog::getExistingDirectory(this, tr("Add Media Folder"));
    if (folder.isEmpty()) {
        return;
    }

    QDir dir(folder);
    const QFileInfoList entries = dir.entryInfoList(supportedNameFilters(), QDir::Files, QDir::Name);
    QStringList paths;
    paths.reserve(entries.size());
    for (const QFileInfo &entry : entries) {
        paths.append(entry.absoluteFilePath());
    }
    addFilePaths(paths);
}

void LocalFileListWidget::removeSelected() {
    const QList<QListWidgetItem *> selected = fileList->selectedItems();
    for (QListWidgetItem *item : selected) {
        delete fileList->takeItem(fileList->row(item));
    }
    updateEmptyState();
}

void LocalFileListWidget::clearFiles() {
    fileList->clear();
    updateEmptyState();
}

void LocalFileListWidget::playItem(QListWidgetItem *item) {
    if (!item) {
        return;
    }

    const QString path = item->data(kPathRole).toString();
    if (!path.isEmpty()) {
        emit playFileRequested(path);
    }
}

void LocalFileListWidget::selectFilePath(const QString &filePath) {
    for (int i = 0; i < fileList->count(); ++i) {
        QListWidgetItem *item = fileList->item(i);
        if (item->data(kPathRole).toString() == filePath) {
            fileList->setCurrentItem(item);
            fileList->scrollToItem(item);
            return;
        }
    }
}

void LocalFileListWidget::setNowPlayingFilePath(const QString &filePath) {
    for (int i = 0; i < fileList->count(); ++i) {
        QListWidgetItem *item = fileList->item(i);
        const QString baseName = item->data(kBaseNameRole).toString();
        const bool isPlaying = !filePath.isEmpty() && item->data(kPathRole).toString() == filePath;

        item->setText(isPlaying ? QString("[Playing] %1").arg(baseName) : baseName);
        QFont itemFont = item->font();
        itemFont.setBold(isPlaying);
        item->setFont(itemFont);
        const ThemePalette palette = ThemeManager::currentPalette();
        item->setBackground(isPlaying ? QColor(palette.checkedBg) : QColor(Qt::transparent));
        item->setForeground(QColor(isPlaying ? palette.textPrimary : palette.textPrimary));

        if (isPlaying) {
            fileList->setCurrentItem(item);
            fileList->scrollToItem(item);
        }
    }
}

void LocalFileListWidget::applyTheme() {
    const ThemePalette palette = ThemeManager::currentPalette();
    setStyleSheet(ThemeManager::localFileListStyle());
    titleLabel->setStyleSheet(QString("color: %1; font-weight: 700;").arg(palette.textPrimary));
    emptyLabel->setStyleSheet(QString("color: %1; padding: 10px;").arg(palette.textMuted));
    for (int i = 0; i < fileList->count(); ++i) {
        QListWidgetItem *item = fileList->item(i);
        const bool isPlaying = item->font().bold();
        item->setBackground(isPlaying ? QColor(palette.checkedBg) : QColor(Qt::transparent));
        item->setForeground(QColor(palette.textPrimary));
    }
}

QString LocalFileListWidget::nextFilePath(const QString &filePath) const {
    if (fileList->count() == 0) {
        return QString();
    }

    int currentIndex = -1;
    for (int i = 0; i < fileList->count(); ++i) {
        if (fileList->item(i)->data(kPathRole).toString() == filePath) {
            currentIndex = i;
            break;
        }
    }

    const int nextIndex = (currentIndex < 0) ? 0 : ((currentIndex + 1) % fileList->count());
    return fileList->item(nextIndex)->data(kPathRole).toString();
}

void LocalFileListWidget::addFilePath(const QString &filePath) {
    const QFileInfo info(filePath);
    if (!info.exists() || !info.isFile()) {
        return;
    }

    const QString canonicalPath = info.canonicalFilePath();
    for (int i = 0; i < fileList->count(); ++i) {
        if (fileList->item(i)->data(kPathRole).toString() == canonicalPath) {
            return;
        }
    }

    auto *item = new QListWidgetItem(info.fileName());
    item->setToolTip(canonicalPath);
    item->setData(kPathRole, canonicalPath);
    item->setData(kBaseNameRole, info.fileName());
    fileList->addItem(item);
}

void LocalFileListWidget::addFilePaths(const QStringList &filePaths) {
    for (const QString &filePath : filePaths) {
        if (fileList->count() >= kMaxStoredFiles) {
            break;
        }
        addFilePath(filePath);
    }
    updateEmptyState();
}

QStringList LocalFileListWidget::supportedNameFilters() const {
    return {
        "*.mp4", "*.mkv", "*.avi", "*.mov", "*.wmv", "*.flv",
        "*.webm", "*.m4v", "*.ts", "*.m2ts", "*.mpg", "*.mpeg"
    };
}

QStringList LocalFileListWidget::filePaths() const {
    QStringList paths;
    paths.reserve(fileList->count());
    for (int i = 0; i < fileList->count(); ++i) {
        paths.append(fileList->item(i)->data(kPathRole).toString());
    }
    return paths;
}

void LocalFileListWidget::updateEmptyState() {
    const bool empty = fileList->count() == 0;
    emptyLabel->setVisible(empty);
    removeButton->setEnabled(!empty);
    clearButton->setEnabled(!empty);
}

void LocalFileListWidget::loadSettings() {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    addFilePaths(settings.value("localFileList", QStringList()).toStringList());
}

void LocalFileListWidget::saveSettings() const {
    QSettings settings("HywelStar", "HywelStarVideoPlayer");
    settings.setValue("localFileList", filePaths());
}
