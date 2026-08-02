/**
 * @file StreamParser.cpp
 * @brief Stream URI parsing implementation
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#include "StreamParser.h"
#include <QUrl>

StreamType StreamParser::detectStreamType(const QString &uri) {
    if (uri.startsWith("rtsp://", Qt::CaseInsensitive)) {
        return StreamType::RTSP;
    } else if (uri.startsWith("udp://", Qt::CaseInsensitive)) {
        return StreamType::UDP;
    } else if (uri.startsWith("tcp://", Qt::CaseInsensitive)) {
        return StreamType::TCP;
    } else if (uri.startsWith("rtmp://", Qt::CaseInsensitive) ||
               uri.startsWith("rtmps://", Qt::CaseInsensitive) ||
               uri.startsWith("rtmpt://", Qt::CaseInsensitive)) {
        return StreamType::RTMP;
    } else if (uri.startsWith("srt://", Qt::CaseInsensitive)) {
        return StreamType::SRT;
    } else if (uri.startsWith("http://", Qt::CaseInsensitive) || uri.startsWith("https://", Qt::CaseInsensitive)) {
        const QUrl url(uri);
        const QString path = url.path().toLower();
        if (path.endsWith(".m3u8")) {
            return StreamType::HLS;
        }
        if (path.endsWith(".mpd")) {
            return StreamType::DASH;
        }
        if (path.endsWith(".mjpg") || path.endsWith(".mjpeg") || uri.contains("mjpeg", Qt::CaseInsensitive)) {
            return StreamType::MJPEG;
        }
        return StreamType::HTTP;
    } else if (uri.startsWith("/dev/video", Qt::CaseInsensitive) || uri.contains("video=", Qt::CaseInsensitive)) {
        return StreamType::UVC;
    }
    return StreamType::Unknown;
}

StreamInfo StreamParser::parseStreamUri(const QString &uri) {
    StreamInfo info;
    info.type = detectStreamType(uri);
    info.uri = uri;

    QUrl url(uri);
    info.host = url.host();
    info.port = url.port();
    info.path = url.path();

    return info;
}

bool StreamParser::isValidUri(const QString &uri) {
    if (uri.isEmpty()) {
        return false;
    }

    StreamType type = detectStreamType(uri);
    if (type == StreamType::Unknown) {
        return false;
    }

    if (type == StreamType::RTSP || type == StreamType::HTTP || type == StreamType::RTMP ||
        type == StreamType::SRT || type == StreamType::HLS || type == StreamType::DASH ||
        type == StreamType::MJPEG) {
        QUrl url(uri);
        return url.isValid();
    }

    return true;
}
