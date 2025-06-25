#include "core/Log.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// 静态成员变量定义
std::vector<std::string> Log::m_logBuffer;

void Log::Init() {}

void Log::Shutdown() {
  LogToFile();  // 程序结束时将日志写入文件
}

void Log::LogMessage(Level level, const std::string& message) {
  std::string levelStr;
  switch (level) {
    case Level::Trace:
      levelStr = "TRACE";
      break;
    case Level::Debug:
      levelStr = "DEBUG";
      break;
    case Level::Info:
      levelStr = "INFO";
      break;
    case Level::Warning:
      levelStr = "WARNING";
      break;
    case Level::Error:
      LogToFile();
      levelStr = "ERROR";
      break;
    case Level::Critical:
      levelStr = "CRITICAL";
      break;
  }

  std::string logMessage = "[" + levelStr + "] " + message;
  m_logBuffer.push_back(logMessage);
}

void Log::PrintLogBuffer() {
  for (const auto& log : m_logBuffer) {
    std::cout << log << std::endl;
  }
}

void Log::LogToFile() {
  if (m_logBuffer.empty()) {
    std::cout << "Log buffer is empty, nothing to write." << std::endl;
    return;  // 没有日志需要写入
  }
  try {
    // 使用固定的绝对日志目录路径
    std::filesystem::path logDir =
        std::filesystem::current_path().parent_path().parent_path() / "log";

    if (!std::filesystem::exists(logDir)) {
      std::filesystem::create_directories(logDir);
      std::cout << "Created log directory: " << logDir << std::endl;
    }

    // 生成带时间戳的日志文件名（使用绝对路径）
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
#ifdef _WIN32
    // Windows 使用安全版本
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << "renderer_" << std::put_time(&timeinfo, "%Y%m%d_%H%M%S") << ".log";
#else
    // Linux/macOS 使用标准版本
    ss << "renderer_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S")
       << ".log";
#endif

    // 创建完整的绝对路径
    std::filesystem::path logFilePath = logDir / ss.str();

    std::ofstream logFile(logFilePath, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
      std::cerr << "Failed to open log file: " << logFilePath << std::endl;
      std::cerr << "Current working directory: "
                << std::filesystem::current_path() << std::endl;

      // 尝试写入到当前目录作为备选方案
      std::filesystem::path fallbackPath =
          std::filesystem::current_path() / ("renderer_fallback_" + ss.str());

      logFile.open(fallbackPath, std::ios::out | std::ios::app);
      if (!logFile.is_open()) {
        std::cerr << "Fallback file creation also failed!" << std::endl;
        return;
      } else {
        std::cout << "Using fallback log file: " << fallbackPath << std::endl;
      }
    }

    // 写入日志头信息
    logFile << "=== PBR Renderer Log File ===" << std::endl;
#ifdef _WIN32
    struct tm headerTimeinfo;
    localtime_s(&headerTimeinfo, &time_t);
    logFile << "Created: "
            << std::put_time(&headerTimeinfo, "%Y-%m-%d %H:%M:%S") << std::endl;
#else
    logFile << "Created: "
            << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
            << std::endl;
#endif
    logFile << "=============================" << std::endl << std::endl;

    // 写入所有日志条目
    for (const auto& logEntry : m_logBuffer) {
      // 添加时间戳到每个日志条目
      auto entryTime = std::chrono::system_clock::now();
      auto entryTimeT = std::chrono::system_clock::to_time_t(entryTime);

#ifdef _WIN32
      struct tm entryTimeinfo;
      localtime_s(&entryTimeinfo, &entryTimeT);
      logFile << "[" << std::put_time(&entryTimeinfo, "%H:%M:%S") << "] "
              << logEntry << std::endl;
#else
      logFile << "[" << std::put_time(std::localtime(&entryTimeT), "%H:%M:%S")
              << "] " << logEntry << std::endl;
#endif
    }
    logFile << std::endl << "=== End of Log ===" << std::endl;
    logFile.close();

    // 清空缓冲区
    m_logBuffer.clear();

  } catch (const std::exception& e) {
    std::cerr << "Error writing log to file: " << e.what() << std::endl;
  }
}
