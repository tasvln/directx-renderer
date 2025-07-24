#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <Windows.h>
#include <cstdarg>

enum class LogType {
    Info,
    Warning,
    Error,
    Debug
};

class Logger {
    public:
        static Logger& instance();
        void log(LogType level, const char* file, const char* function, int line, const wchar_t* format, ...);
        void setLogFilePath(const std::wstring& path);

    private:
        Logger();
        ~Logger();

        void writeLog(const std::wstring& message);
        std::wstring formatLogMessage(LogType level, const std::wstring& msg, const char* file, const char* function, int line);

        std::wofstream file;
        std::mutex mutex;
};