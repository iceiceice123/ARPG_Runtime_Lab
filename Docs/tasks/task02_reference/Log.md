# Task 2 参考答案：Log（卡住时再看）

> 使用规则：默认你自己写。卡住 ~15 分钟，或你主动要求时，再来看这里。
> 看的时候别抄，理解每一行为什么这样写，然后合上文件自己敲。

## 设计要点（先读，再决定要不要看代码）

1. **Sink 抽象**：日志不直接 printf，而是通过 `ILogSink` 接口输出。默认 `StdoutLogSink`，测试换成 `MemoryLogSink` 就能捕获日志内容并断言。
2. **宏 + 函数分层**：`ARPG_LOG` 宏负责"级别过滤 + 捕获调用点"，`LogMessage` 函数负责"格式化 + 分发到 sink"。宏里的 `if (IsEnabled)` 保证关闭的级别**连参数都不求值**（零开销）。
3. **格式化用 std::format**（类型安全），输出层用 printf 的 `%.*s`（对 string_view 友好）。
4. **位置用 std::source_location**（C++20），不用 `__FILE__`/`__LINE__`。

## Log.hpp

```cpp
#pragma once

#include <cstdint>
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace arpg
{
    enum class LogLevel : std::uint8_t
    {
        Verbose = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3,
    };

    constexpr const char* ToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Verbose: return "Verbose";
        case LogLevel::Info:    return "Info";
        case LogLevel::Warning: return "Warning";
        case LogLevel::Error:   return "Error";
        }
        return "Unknown";
    }

    class ILogSink
    {
    public:
        virtual ~ILogSink() = default;
        virtual void Write(LogLevel level,
                           std::string_view message,
                           std::string_view file,
                           int line) = 0;
    };

    class StdoutLogSink final : public ILogSink
    {
    public:
        void Write(LogLevel level,
                   std::string_view message,
                   std::string_view file,
                   int line) override;
    };

    class MemoryLogSink final : public ILogSink
    {
    public:
        struct Entry
        {
            LogLevel level;
            std::string message;
            std::string file;
            int line = 0;
        };

        void Write(LogLevel level,
                   std::string_view message,
                   std::string_view file,
                   int line) override;

        const std::vector<Entry>& Entries() const { return entries; }
        void Clear() { entries.clear(); }

    private:
        std::vector<Entry> entries;
    };

    LogLevel GetMinLogLevel();
    void SetMinLogLevel(LogLevel level);
    void SetLogSink(ILogSink* sink);   // nullptr = 恢复默认 stdout
    ILogSink* GetLogSink();

    namespace detail
    {
        bool IsEnabled(LogLevel level);
    }

    // 注意：source_location 放在参数包【前面】，否则 MSVC 推导会出错。
    template <typename... Args>
    void LogMessage(LogLevel level,
                    std::source_location loc,
                    std::string_view fmt,
                    const Args&... args)
    {
        if (!detail::IsEnabled(level))
        {
            return;
        }

        const std::string message = std::vformat(fmt, std::make_format_args(args...));

        ILogSink* sink = GetLogSink();
        if (sink != nullptr)
        {
            sink->Write(level, message, loc.file_name(), static_cast<int>(loc.line()));
        }
    }

} // namespace arpg

#define ARPG_LOG(level, ...)                                            \
    do                                                                  \
    {                                                                   \
        if (::arpg::detail::IsEnabled(level))                           \
        {                                                               \
            ::arpg::LogMessage(level,                                   \
                               ::std::source_location::current(),       \
                               __VA_ARGS__);                            \
        }                                                               \
    } while (0)

#define ARPG_LOG_VERBOSE(...) ARPG_LOG(::arpg::LogLevel::Verbose, __VA_ARGS__)
#define ARPG_LOG_INFO(...)    ARPG_LOG(::arpg::LogLevel::Info, __VA_ARGS__)
#define ARPG_LOG_WARNING(...) ARPG_LOG(::arpg::LogLevel::Warning, __VA_ARGS__)
#define ARPG_LOG_ERROR(...)   ARPG_LOG(::arpg::LogLevel::Error, __VA_ARGS__)
```

## Log.cpp

```cpp
#include <arpg/Log.hpp>

#include <cstdio>

namespace arpg
{
    namespace
    {
        LogLevel g_minLevel = LogLevel::Verbose;
        ILogSink* g_sink = nullptr;
        StdoutLogSink g_defaultSink;
    }

    void StdoutLogSink::Write(LogLevel level,
                              std::string_view message,
                              std::string_view file,
                              int line)
    {
        std::printf("[%-7s] %.*s (%.*s:%d)\n",
                    ToString(level),
                    static_cast<int>(message.size()), message.data(),
                    static_cast<int>(file.size()), file.data(),
                    line);
    }

    void MemoryLogSink::Write(LogLevel level,
                              std::string_view message,
                              std::string_view file,
                              int line)
    {
        entries.push_back({ level, std::string(message), std::string(file), line });
    }

    LogLevel GetMinLogLevel() { return g_minLevel; }
    void SetMinLogLevel(LogLevel level) { g_minLevel = level; }
    void SetLogSink(ILogSink* sink) { g_sink = sink; }

    ILogSink* GetLogSink()
    {
        return g_sink != nullptr ? g_sink : &g_defaultSink;
    }

    namespace detail
    {
        bool IsEnabled(LogLevel level)
        {
            return static_cast<std::uint8_t>(level) >= static_cast<std::uint8_t>(g_minLevel);
        }
    }
} // namespace arpg
```

## 三个"为什么"（看完代码，自测能不能答上来）

1. 为什么 `LogMessage` 的 `std::source_location` 参数要放在 `const Args&...` **前面**？
2. 为什么宏里的 `if (IsEnabled)` 和函数里的 `if (!IsEnabled) return` 要**各写一次**（不能只留一个）？
3. `%.*s` 为什么不直接写 `%s`？

答不上来就是还没懂，回去重看，别急着抄进正式代码。
