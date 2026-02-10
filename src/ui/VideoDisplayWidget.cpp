/**
 * @file VideoDisplayWidget.cpp
 * @brief Video display widget implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "VideoDisplayWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEnterEvent>

VideoDisplayWidget::VideoDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #0d0d1a;");
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void VideoDisplayWidget::setAspectRatio(AspectRatioMode mode) {
    aspectMode = mode;
    update();
}

void VideoDisplayWidget::setScaleMode(ScaleMode mode) {
    scaleMode = mode;
    update();
}

void VideoDisplayWidget::showGrid(bool show) {
    gridVisible = show;
    update();
}

void VideoDisplayWidget::toggleGrid() {
    gridVisible = !gridVisible;
    update();
}

void VideoDisplayWidget::zoom(int delta) {
    zoomLevel += delta * 0.1f;
    if (zoomLevel < 0.1f) zoomLevel = 0.1f;
    if (zoomLevel > 5.0f) zoomLevel = 5.0f;
    update();
}

void VideoDisplayWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#0d0d1a"));

    if (gridVisible) {
        drawGrid(painter);
    }

    if (showHelpText) {
        drawLoadingAnimation(painter);
    }
}

void VideoDisplayWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPos = event->pos();
    }
}

void VideoDisplayWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        // Handle panning
    }
}

void VideoDisplayWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}

void VideoDisplayWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit fullScreenRequested();
    }
}

void VideoDisplayWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        zoom(1);
        break;
    case Qt::Key_Minus:
        zoom(-1);
        break;
    default:
        // Pass all other keys to parent (MainWindow handles global shortcuts)
        QWidget::keyPressEvent(event);
    }
}

void VideoDisplayWidget::enterEvent(QEnterEvent *event) {
    showHelpText = true;
    update();
}

void VideoDisplayWidget::leaveEvent(QEvent *event) {
    showHelpText = false;
    update();
}

void VideoDisplayWidget::drawGrid(QPainter &painter) {
    painter.setPen(QPen(QColor(255, 255, 255, 100), 1));

    int gridSpacing = 60;

    // Draw vertical lines
    for (int x = 0; x < width(); x += gridSpacing) {
        painter.drawLine(x, 0, x, height());
    }

    // Draw horizontal lines
    for (int y = 0; y < height(); y += gridSpacing) {
        painter.drawLine(0, y, width(), y);
    }

    // Draw center crosshair
    painter.setPen(QPen(QColor(255, 0, 0, 150), 2));
    int centerX = width() / 2;
    int centerY = height() / 2;
    painter.drawLine(centerX - 20, centerY, centerX + 20, centerY);
    painter.drawLine(centerX, centerY - 20, centerX, centerY + 20);
}

void VideoDisplayWidget::drawLoadingAnimation(QPainter &painter) {
    painter.setPen(QPen(QColor(255, 255, 255, 200), 1));
    painter.setFont(QFont("Arial", 12));

    QString helpText = "F-Fullscreen | G-Grid | +/- Zoom | Space-Play/Pause | R-Record | S-Screenshot";
    QRect textRect = painter.fontMetrics().boundingRect(helpText);
    textRect.moveTo(10, height() - 30);

    painter.drawText(textRect, Qt::AlignLeft, helpText);
}
