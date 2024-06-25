#pragma once
#include <string>
#include <vector>

enum LogType
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEntry
{
    LogType type;
    std::string message;
};

class Logger
{
private:
    static std::string monthStr;
    static std::string hourStr;
    static std::string minStr;
    static std::string secStr;

public:
    static std::vector<LogEntry> messages;
    static void Log(const std::string &message);
    static void Err(const std::string &message);
};
