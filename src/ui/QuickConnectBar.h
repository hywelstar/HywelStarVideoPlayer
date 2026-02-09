/**
 * @file QuickConnectBar.h
 * @brief Quick connection bar for stream URI input
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef QUICK_CONNECT_BAR_H
#define QUICK_CONNECT_BAR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class QuickConnectBar : public QWidget {
    Q_OBJECT

public:
    QuickConnectBar(QWidget *parent = nullptr);

    QString getStreamUri() const;
    void addToHistory(const QString &uri);
    void setUri(const QString &uri);

signals:
    void connectRequested(const QString &uri);
    void disconnectRequested();
    void settingsRequested();

private:
    void setupUI();
    void connectSignals();
    void loadHistory();

    QLabel *titleLabel;
    QLineEdit *uriInput;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *settingsButton;

    bool isConnected = false;
};

#endif // QUICK_CONNECT_BAR_H
