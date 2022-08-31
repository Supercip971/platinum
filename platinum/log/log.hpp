#pragma once
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
namespace plt
{
struct Log
{
private:
    //    const std::string output = "/dev/stdout";
    //    const std::string error = "/dev/stderr";

public:
    enum class Level
    {
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    template <typename S, typename... Args>
    static void log(Level level, const char *file, int line, S msg, Args &&...args)
    {
        switch (level)
        {
        case Level::Debug:
            fmt::print(fmt::fg(fmt::color::white_smoke) | fmt::emphasis::bold, "[DEBUG]");
            fmt::print(fmt::fg(fmt::color::dim_gray), " {}:{}: ", file, line);
            fmt::print("{}\n", fmt::format(msg, args...));
            break;

        case Level::Info:
            fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "[INFO ]");
            fmt::print(fmt::fg(fmt::color::dim_gray), " {}:{}: ", file, line);
            fmt::print("{}\n", fmt::format(msg, args...));
            break;

        case Level::Warn:
            fmt::print(fmt::fg(fmt::color::orange) | fmt::emphasis::bold, "[WARN ]");
            fmt::print(fmt::fg(fmt::color::dim_gray), " {}:{}: ", file, line);
            fmt::print("{}\n", fmt::format(msg, args...));
            break;

        case Level::Error:
            fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold, "[ERROR]");
            fmt::print(fmt::fg(fmt::color::dim_gray), " {}:{}: ", file, line);
            fmt::print("{}\n", fmt::format(msg, args...));
            break;

        case Level::Fatal:
            fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "{} {}:{}: {}\n", "[FATAL]", file, line, fmt::format(msg, args...));
            exit(-1);
            break;
        }
    }
}; // namespace plat

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define debug$(msg, ...) plt::Log::log(plt::Log::Level::Debug, __FILENAME__, __LINE__, FMT_STRING(msg), ##__VA_ARGS__)
#define info$(msg, ...) plt::Log::log(plt::Log::Level::Info, __FILENAME__, __LINE__, FMT_STRING(msg), ##__VA_ARGS__)
#define warn$(msg, ...) plt::Log::log(plt::Log::Level::Warn, __FILENAME__, __LINE__, FMT_STRING(msg), ##__VA_ARGS__)
#define error$(msg, ...) plt::Log::log(plt::Log::Level::Error, __FILENAME__, __LINE__, FMT_STRING(msg), ##__VA_ARGS__)
#define fatal$(msg, ...) plt::Log::log(plt::Log::Level::Fatal, __FILENAME__, __LINE__, FMT_STRING(msg), ##__VA_ARGS__)

}; // namespace plt
