#include "pch.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Logger() {
    // Default log path - create logs folder and app.log
    std::wstring logDir = L"logs";
    CreateDirectoryW(logDir.c_str(), nullptr);
    setLogFilePath(logDir + L"\\engine.log");
}

Logger::~Logger() {
    if (file.is_open())
        file.close();
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFilePath(const std::wstring& path) {
    std::lock_guard<std::mutex> lock(mutex);
    if (file.is_open())
        file.close();

    file.open(path, std::ios_base::app);
}

void Logger::log(LogType level, const char* file, const char* function, int line, const wchar_t* format, ...) {
    wchar_t buffer[4096];

    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);
    va_end(args);

    std::wstring message = formatLogMessage(level, buffer, file, function, line);

    OutputDebugStringW(message.c_str());
    writeLog(message);
}


void Logger::writeLog(const std::wstring& message) {
    std::lock_guard<std::mutex> lock(mutex);
    if (file.is_open()) {
        file << message;
        file.flush();
    }
}

std::wstring Logger::formatLogMessage(LogType level, const std::wstring& msg, const char* file, const char* function, int line) {
    auto now = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime, &timeT);

    std::wstringstream ss;
    ss << L"[";
    ss << std::put_time(&localTime, L"%Y-%m-%d %H:%M:%S");
    ss << L"] ";

    switch (level) {
        case LogType::Info:    ss << L"[INFO]    "; break;
        case LogType::Warning: ss << L"[WARNING] "; break;
        case LogType::Error:   ss << L"[ERROR]   "; break;
        case LogType::Debug:   ss << L"[DEBUG]   "; break;
        default:               ss << L"[LOG]     "; break;
    }

    // Convert char* file/function to wstring
    ss << L"[" << std::filesystem::path(file).filename().wstring() << L":" << line << L"] ";
    ss << msg << L"\n";
    return ss.str();
}

