/**
 * @file AboutDialog.h
 * @brief About dialog showing application information
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
};

#endif // ABOUT_DIALOG_H
