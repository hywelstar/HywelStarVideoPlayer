/**
 * @file VideoDisplayWidget.cpp
 * @brief Video display widget implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "VideoDisplayWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEnterEvent>
#include <QSlider>
#include <QTimer>

VideoDisplayWidget::VideoDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #000000;");
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMouseTracking(true);
    setupOverlay();

    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    connect(clickTimer, &QTimer::timeout, this, [this]() {
        emit playPauseRequested();
    });
}

void VideoDisplayWidget::setupOverlay() {
    progressOverlay = new QWidget(this);
    progressOverlay->setVisible(false);
    progressOverlay->setStyleSheet(R"(
        QWidget {
            background-color: rgba(0, 0, 0, 150);
            border-radius: 6px;
        }
        QLabel {
            color: #FFFFFF;
            background: transparent;
        }
        QSlider {
            background: transparent;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 5px;
            background: rgba(255, 255, 255, 90);
            border-radius: 2px;
        }
        QSlider::sub-page:horizontal {
            background: #DDE7F8;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            border: none;
            width: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
    )");

    auto *layout = new QHBoxLayout(progressOverlay);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(10);

    positionLabel = new QLabel("00:00", progressOverlay);
    positionLabel->setMinimumWidth(46);
    layout->addWidget(positionLabel);

    positionSlider = new QSlider(Qt::Horizontal, progressOverlay);
    positionSlider->setMinimum(0);
    positionSlider->setMaximum(0);
    positionSlider->setEnabled(false);
    layout->addWidget(positionSlider, 1);

    durationLabel = new QLabel("--:--", progressOverlay);
    durationLabel->setMinimumWidth(46);
    layout->addWidget(durationLabel);

    overlayHideTimer = new QTimer(this);
    overlayHideTimer->setSingleShot(true);
    connect(overlayHideTimer, &QTimer::timeout, this, &VideoDisplayWidget::hideProgressOverlay);
    connect(positionSlider, &QSlider::sliderPressed, this, &VideoDisplayWidget::showProgressOverlay);
    connect(positionSlider, &QSlider::sliderReleased, this, [this]() {
        emit seekRequested(positionSlider->value() * 1000LL);
        showProgressOverlay();
    });
    connect(positionSlider, &QSlider::sliderMoved, this, [this](int value) {
        if (!isUpdatingPosition) {
            positionLabel->setText(formatTime(value * 1000LL));
        }
        showProgressOverlay();
    });

    positionOverlay();
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

void VideoDisplayWidget::setPosition(qint64 positionMs, qint64 durationMs) {
    const bool hasDuration = durationMs > 0;
    isUpdatingPosition = true;
    positionSlider->setEnabled(hasDuration);
    positionSlider->setMaximum(hasDuration ? static_cast<int>(durationMs / 1000) : 0);
    if (hasDuration && !positionSlider->isSliderDown()) {
        positionSlider->setValue(static_cast<int>(qBound<qint64>(0, positionMs / 1000, durationMs / 1000)));
    }
    isUpdatingPosition = false;

    positionLabel->setText(positionMs >= 0 ? formatTime(positionMs) : "00:00");
    durationLabel->setText(hasDuration ? formatTime(durationMs) : "--:--");
}

void VideoDisplayWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#000000"));

    if (gridVisible) {
        drawGrid(painter);
    }

    if (showHelpText) {
        drawLoadingAnimation(painter);
    }
}

void VideoDisplayWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    positionOverlay();
}

void VideoDisplayWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPos = event->pos();
        suppressClickToggle = false;
        showProgressOverlay();
    }
}

void VideoDisplayWidget::mouseMoveEvent(QMouseEvent *event) {
    showProgressOverlay();
    if (isDragging) {
        if ((event->pos() - dragStartPos).manhattanLength() > 6) {
            suppressClickToggle = true;
        }
    }
    QWidget::mouseMoveEvent(event);
}

void VideoDisplayWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        showProgressOverlay();
        if (!suppressClickToggle && (event->pos() - dragStartPos).manhattanLength() <= 6) {
            clickTimer->start(180);
        }
        isDragging = false;
    }
}

void VideoDisplayWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        suppressClickToggle = true;
        if (clickTimer) {
            clickTimer->stop();
        }
        showProgressOverlay();
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
    Q_UNUSED(event)
    showHelpText = true;
    showProgressOverlay();
    update();
}

void VideoDisplayWidget::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
    showHelpText = false;
    hideProgressOverlay();
    update();
}

void VideoDisplayWidget::positionOverlay() {
    if (!progressOverlay) {
        return;
    }

    const int margin = 18;
    const int overlayHeight = 42;
    progressOverlay->setGeometry(margin,
                                 qMax(margin, height() - overlayHeight - margin),
                                 qMax(0, width() - margin * 2),
                                 overlayHeight);
}

void VideoDisplayWidget::showProgressOverlay() {
    if (!progressOverlay) {
        return;
    }

    positionOverlay();
    progressOverlay->show();
    progressOverlay->raise();
    if (overlayHideTimer) {
        overlayHideTimer->start(3000);
    }
}

void VideoDisplayWidget::hideProgressOverlay() {
    if (positionSlider && positionSlider->isSliderDown()) {
        showProgressOverlay();
        return;
    }
    if (progressOverlay) {
        progressOverlay->hide();
    }
}

QString VideoDisplayWidget::formatTime(qint64 milliseconds) const {
    if (milliseconds < 0) {
        return "--:--";
    }

    qint64 seconds = milliseconds / 1000;
    const qint64 hours = seconds / 3600;
    seconds %= 3600;
    const qint64 minutes = seconds / 60;
    seconds %= 60;

    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
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
    textRect.adjust(-8, -4, 8, 4);
    textRect.moveTo(12, 12);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 130));
    painter.drawRoundedRect(textRect, 4, 4);

    painter.setPen(QPen(QColor(255, 255, 255, 210), 1));
    painter.drawText(textRect, Qt::AlignLeft, helpText);
}
