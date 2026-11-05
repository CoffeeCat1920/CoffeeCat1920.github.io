#pragma once

#include <iostream>
#include <string_view>
#include <format>
#include <chrono>

namespace logger {

enum class Level {
  LOG,
  WARNING,
  ERROR
};

// ANSI color escape codes
inline constexpr std::string_view COLOR_RESET   = "\033[0m";
inline constexpr std::string_view COLOR_LOG     = "\033[32m"; // Green
inline constexpr std::string_view COLOR_WARNING = "\033[33m"; // Yellow
inline constexpr std::string_view COLOR_ERROR   = "\033[31m"; // Red
inline constexpr std::string_view COLOR_MUTED   = "\033[90m"; // Gray

inline void LogMessage(Level level, std::string_view msg, std::string_view file = "", int line = 0) {
  std::ostream& out = (level == Level::ERROR) ? std::cerr : std::cout;

  // Format current timestamp HH:MM:SS
  const auto now = std::chrono::system_clock::now();
  const auto time = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::floor<std::chrono::seconds>(now)};

  switch (level) {
    case Level::LOG:
      out << COLOR_MUTED << std::format("[{:%T}]", time) << " "
          << COLOR_LOG << "[LOG] "
          << COLOR_RESET << msg << "\n";
      break;

    case Level::WARNING:
      out << COLOR_MUTED << std::format("[{:%T}]", time) << " "
          << COLOR_WARNING << "[WARNING] "
          << COLOR_RESET << msg
          << COLOR_MUTED << std::format(" ({}:{})", file, line)
          << COLOR_RESET << "\n";
      break;

    case Level::ERROR:
      out << COLOR_MUTED << std::format("[{:%T}]", time) << " "
          << COLOR_ERROR << "[ERROR] "
          << COLOR_RESET << msg
          << COLOR_MUTED << std::format(" ({}:{})", file, line)
          << COLOR_RESET << "\n";
      break;
  }
}

template <typename... Args>
void Log(std::format_string<Args...> fmt, Args&&... args) {
  LogMessage(Level::LOG, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void Warning(std::string_view file, int line, std::format_string<Args...> fmt, Args&&... args) {
  LogMessage(Level::WARNING, std::format(fmt, std::forward<Args>(args)...), file, line);
}

template <typename... Args>
void Error(std::string_view file, int line, std::format_string<Args...> fmt, Args&&... args) {
  LogMessage(Level::ERROR, std::format(fmt, std::forward<Args>(args)...), file, line);
}

} // namespace logger

// Macros for logging
// LOG: standard message without line/file clutter
#define LOG(...) ::logger::Log(__VA_ARGS__)

// WARNING & ERROR: fancy macros capturing file and line number
#define LOG_WARNING(...) ::logger::Warning(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) ::logger::Error(__FILE__, __LINE__, __VA_ARGS__)
