/**
 * @file StreamParser.h
 * @brief Stream URI parsing and type detection
 * @author hywelstar
 * @email hywelstar@126.com
 * @date 2026
 * @copyright MIT License
 */

#ifndef STREAM_PARSER_H
#define STREAM_PARSER_H

#include <QString>

enum class StreamType {
    RTSP,
    UDP,
    TCP,
    HTTP,
    MJPEG,
    UVC,
    Unknown
};

struct StreamInfo {
    StreamType type;
    QString uri;
    QString host;
    int port = 0;
    QString path;
};

class StreamParser {
public:
    static StreamType detectStreamType(const QString &uri);
    static StreamInfo parseStreamUri(const QString &uri);
    static bool isValidUri(const QString &uri);
};

#endif // STREAM_PARSER_H
