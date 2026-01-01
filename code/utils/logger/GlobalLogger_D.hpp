#pragma once

#include <windows.h>
#include <memory>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>

#include <Logger.hpp>
#include <fstream>

enum class DefaultLevel {
    Debug = 0,
    Info,
    Warn,
    Error
};

struct DefaultInfo {
    std::string timestamp;
    std::string source;
    int threadId;

    DefaultInfo() : source("unknown"), threadId(0) {
        // 生成时间戳
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        timestamp = oss.str();
    }

    DefaultInfo(std::string src) : source(std::move(src)), threadId(0) {
        // 生成时间戳
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        timestamp = oss.str();
    }
};

class GlobalLogger {
public:
    /**
     * @brief 构造函数
     * @param level 日志级别过滤
     */
    explicit GlobalLogger(DefaultLevel level = DefaultLevel::Info) {
        if (_instance != nullptr) return;
        _minLevel = level;
        _instance = std::make_unique<Logger<DefaultLevel, DefaultInfo>>(
            Logger<DefaultLevel, DefaultInfo>::builder()
                .appendFilter(levelFilter)
                .formatter(format)
                .appendHandler(consoleHandler)
                .build()
        );
    }

    /**
     * @brief 获取日志器实例
     */
    static Logger<DefaultLevel, DefaultInfo>& instance() {
        if (_instance == nullptr) {
            // 使用默认级别创建实例
            static GlobalLogger init(DefaultLevel::Info);
        }
        return *_instance;
    }

    /**
     * @brief 设置最小日志级别
     */
    static void setMinLevel(DefaultLevel level) {
        _minLevel = level;
    }

    /**
     * @brief 级别过滤器
     */
    static bool levelFilter(const Logger<DefaultLevel, DefaultInfo>::LogRecord& record) {
        return static_cast<int>(record.level) >= static_cast<int>(_minLevel);
    }

    /**
     * @brief 格式化器
     */
    static std::string format(const Logger<DefaultLevel, DefaultInfo>::LogRecord& record) {
        std::string levelStr;
        switch (record.level) {
            case DefaultLevel::Debug: levelStr = "DEBUG"; break;
            case DefaultLevel::Info:  levelStr = "INFO";  break;
            case DefaultLevel::Warn:  levelStr = "WARN";  break;
            case DefaultLevel::Error: levelStr = "ERROR"; break;
        }

        return record.additionInfo.timestamp +
               " [" + levelStr + "] " +
               "[" + record.additionInfo.source + "] " +
               record.message;
    }

    /**
     * @brief 控制台处理器
     */
    static void consoleHandler(Logger<DefaultLevel, DefaultInfo>::LogRecord& record,
                               const std::string& str) {
        // 根据不同级别使用不同颜色输出（仅限支持ANSI的终端）
        #ifdef _WIN32
            // Windows 控制台颜色
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD originalColor;
            CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
            GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
            originalColor = consoleInfo.wAttributes;

            switch (record.level) {
                case DefaultLevel::Error:
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                    break;
                case DefaultLevel::Warn:
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    break;
                case DefaultLevel::Info:
                    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
                    break;
                case DefaultLevel::Debug:
                    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
                    break;
            }

            std::cout << str << std::endl;
            SetConsoleTextAttribute(hConsole, originalColor);
        #else
            // Linux/Mac ANSI颜色
            switch (record.level) {
                case DefaultLevel::Error:
                    std::cout << "\033[1;31m" << str << "\033[0m" << std::endl;
                    break;
                case DefaultLevel::Warn:
                    std::cout << "\033[1;33m" << str << "\033[0m" << std::endl;
                    break;
                case DefaultLevel::Info:
                    std::cout << "\033[1;32m" << str << "\033[0m" << std::endl;
                    break;
                case DefaultLevel::Debug:
                    std::cout << "\033[36m" << str << "\033[0m" << std::endl;
                    break;
            }
        #endif
    }

    /**
     * @brief 文件处理器
     */
    static void fileHandler(Logger<DefaultLevel, DefaultInfo>::LogRecord& record,
                           const std::string& str) {
        static std::ofstream logFile("all.log");
        if (logFile.is_open()) {
            logFile << str << std::endl;
        }
    }

    /**
     * @brief 添加文件处理器
     */
    static void addFileHandler() {
        // 创建新的日志器实例并添加文件处理器
        auto newLogger = Logger<DefaultLevel, DefaultInfo>::builder()
            .appendFilter(levelFilter)
            .formatter(format)
            .appendHandler(consoleHandler)
            .appendHandler(fileHandler)
            .build();

        _instance = std::make_unique<Logger<DefaultLevel, DefaultInfo>>(std::move(newLogger));
    }

private:
    static inline std::unique_ptr<Logger<DefaultLevel, DefaultInfo>> _instance = nullptr;
    static inline DefaultLevel _minLevel = DefaultLevel::Info;
};