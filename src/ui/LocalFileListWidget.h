/**
 * @file LocalFileListWidget.h
 * @brief Local media file list widget
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef LOCAL_FILE_LIST_WIDGET_H
#define LOCAL_FILE_LIST_WIDGET_H

#include <QWidget>
#include <QStringList>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class LocalFileListWidget : public QWidget {
    Q_OBJECT

public:
    explicit LocalFileListWidget(QWidget *parent = nullptr);

    void loadSettings();
    void saveSettings() const;
    void selectFilePath(const QString &filePath);
    void setNowPlayingFilePath(const QString &filePath);
    QString nextFilePath(const QString &filePath) const;
    void applyTheme();

signals:
    void playFileRequested(const QString &filePath);

private slots:
    void addFiles();
    void addFolder();
    void removeSelected();
    void clearFiles();
    void playItem(QListWidgetItem *item);

private:
    void setupUI();
    void connectSignals();
    void addFilePath(const QString &filePath);
    void addFilePaths(const QStringList &filePaths);
    QStringList supportedNameFilters() const;
    QStringList filePaths() const;
    void updateEmptyState();

    QLabel *titleLabel;
    QLabel *emptyLabel;
    QListWidget *fileList;
    QPushButton *addFilesButton;
    QPushButton *addFolderButton;
    QPushButton *removeButton;
    QPushButton *clearButton;
};

#endif // LOCAL_FILE_LIST_WIDGET_H
