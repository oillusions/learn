#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <string>


/**
 * @brief 日志器本体
 * @details 他似乎不需要详细注释[划掉]
 * @tparam LogLevelEnum 日志等级
 * @tparam AdditionInfo 日志记录附加信息
 */
template<typename LogLevelEnum, typename AdditionInfo>
class Logger {
    static_assert(std::is_enum_v<LogLevelEnum>, "错误: 日志器模板类型[ LogLevelEnum ]必须为枚举类型");
    static_assert(std::is_default_constructible_v<AdditionInfo>, "错误: 日志器模板类型[ AdditionInfo ]必须实现默认构造");
    public:
        struct LogRecord {
            LogLevelEnum level;
            std::string message;
            AdditionInfo additionInfo;
        };

        using FilterType = std::function<bool(const LogRecord&)>;
        using FormatterType = std::function<std::string(const LogRecord&)>;
        using HandlerType = std::function<void(const LogRecord&, const std::string&)>;


        /**
         * @brief 日志器构建者
         * @details 他似乎不需要详细注释[划掉]
         */
        class LoggerBuilder {
            public:
                LoggerBuilder() = default;
                ~LoggerBuilder() = default;

                /**
                 * @brief 添加日志过滤器
                 * @details 他似乎不需要详细注释[划掉]
                 * @param filter 过滤器回调
                 * @return 构建者引用
                 */
                LoggerBuilder& appendFilter(const FilterType& filter) {
                    filters.push_back(filter);
                    return *this;
                }

                /**
                 * @brief 配置日志格式化器
                 * @details 他似乎不需要详细注释[划掉]
                 * @param formatter 格式化器回调
                 * @return 构建者引用
                 */
                LoggerBuilder& formatter(const FormatterType& formatter) {
                    _formatter = formatter;
                    return *this;
                }

                /**
                 * @brief 添加日志处理器
                 * @details 他似乎不需要详细注释[划掉]
                 * @param handler 处理器回调
                 * @return 构建者引用
                 */
                LoggerBuilder& appendHandler(const HandlerType& handler) {
                    handlers.push_back(handler);
                    return *this;
                }

                /**
                 * @brief 构建日志器
                 * @details 他似乎不需要详细注释[划掉]
                 * @return 日志器
                 */
                Logger build() {
                    return Logger(std::move(filters), std::move(_formatter), std::move(handlers));
                }
            private:
                FormatterType _formatter;
                std::vector<FilterType> filters;
                std::vector<HandlerType> handlers;

                std::queue<LogRecord> _logQueue;


        };

        ~Logger() = default;

        /**
         * @brief 获取构建器
         * @details 他似乎不需要详细注释[划掉]
         * @return 构建器
         */
        static LoggerBuilder builder() {
            return {};
        }

        /**
         * @brief 添加日志
         * @details 他似乎不需要详细注释[划掉]
         * @param level 日志等级
         * @param message 日志消息
         * @param info 日志附加信息
         */
        void log(LogLevelEnum level, const std::string& message, AdditionInfo info = {}) {
            {
                std::lock_guard lock(mtx);
                processRecord(LogRecord{level, message, info});
            }
        }

        /**
         * @brief 添加日志[模板]
         * @details 他似乎不需要详细注释[划掉]
         * @tparam Level 日志等级
         * @param message 日志消息
         * @param info 日志附加信息
         */
        template<LogLevelEnum Level>
        void log(const std::string& message, AdditionInfo info = {}) {
            log(Level, message, info);
        }

        void processRecord(const LogRecord& record) {
            for (auto& e : _filters) {
                if (!e(record)) {return;}
            }
            std::string str = _formatter(record);
            for (auto& e : _handlers) {
                e(record, str);
            }
        }

    private:
        std::vector<FilterType> _filters;
        FormatterType _formatter;
        std::vector<HandlerType> _handlers;

        std::mutex mtx;

        /**
         * @brief 构建者使用的日志器构造
         * @details 他似乎不需要详细注释[划掉]
         * @param filters 过滤器数组
         * @param formatter 格式化器
         * @param handlers 处理器数组
         */
        Logger(std::vector<FilterType> filters,
           FormatterType formatter,
           std::vector<HandlerType> handlers):
                _filters(std::move(filters)),
                _formatter(std::move(formatter)),
                _handlers(std::move(handlers)){
    }
};

