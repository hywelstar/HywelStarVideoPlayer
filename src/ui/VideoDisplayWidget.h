/**
 * @file VideoDisplayWidget.h
 * @brief Video display widget with grid overlay support
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef VIDEO_DISPLAY_WIDGET_H
#define VIDEO_DISPLAY_WIDGET_H

#include <QWidget>
#include <QPoint>

class QLabel;
class QSlider;
class QTimer;

enum class AspectRatioMode {
    Keep,
    Fill,
    Stretch
};

enum class ScaleMode {
    Fit,
    Stretch,
    Crop
};

class VideoDisplayWidget : public QWidget {
    Q_OBJECT

public:
    VideoDisplayWidget(QWidget *parent = nullptr);

    void setAspectRatio(AspectRatioMode mode);
    void setScaleMode(ScaleMode mode);
    void showGrid(bool show);
    void toggleGrid();
    void zoom(int delta);
    void setPosition(qint64 positionMs, qint64 durationMs);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void fullScreenRequested();
    void gridToggleRequested();
    void seekRequested(qint64 positionMs);
    void playPauseRequested();

private:
    void setupOverlay();
    void positionOverlay();
    void showProgressOverlay();
    void hideProgressOverlay();
    QString formatTime(qint64 milliseconds) const;
    void drawGrid(QPainter &painter);
    void drawLoadingAnimation(QPainter &painter);

    QWidget *progressOverlay = nullptr;
    QLabel *positionLabel = nullptr;
    QSlider *positionSlider = nullptr;
    QLabel *durationLabel = nullptr;
    QTimer *overlayHideTimer = nullptr;
    QTimer *clickTimer = nullptr;
    bool gridVisible = false;
    bool isUpdatingPosition = false;
    bool suppressClickToggle = false;
    AspectRatioMode aspectMode = AspectRatioMode::Keep;
    ScaleMode scaleMode = ScaleMode::Fit;
    QPoint dragStartPos;
    bool isDragging = false;
    bool showHelpText = false;
    float zoomLevel = 1.0f;
};

#endif // VIDEO_DISPLAY_WIDGET_H
