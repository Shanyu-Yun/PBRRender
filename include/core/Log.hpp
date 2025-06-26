#pragma once
#include <iostream>
#include <string>
#include <vector>

class Log {
 public:
  Log() = default;
  ~Log() = default;
  enum class Level { Trace, Debug, Info, Warning, Error, Critical };

  static void Init();
  static void Shutdown();

  static void LogMessage(Level level, const std::string& message);
  static void PrintLogBuffer();

  // 便捷的日志方法
  static void Trace(const std::string& message) {
    LogMessage(Level::Trace, message);
  }
  static void Debug(const std::string& message) {
    LogMessage(Level::Debug, message);
  }
  static void Info(const std::string& message) {
    LogMessage(Level::Info, message);
  }
  static void Warning(const std::string& message) {
    LogMessage(Level::Warning, message);
  }
  static void Error(const std::string& message) {
    LogMessage(Level::Error, message);
  }
  static void Critical(const std::string& message) {
    LogMessage(Level::Critical, message);
  }

  // 获取缓冲区大小
  static size_t GetBufferSize() { return m_logBuffer.size(); }

  //立即刷新缓冲保存log文件
  static void FlushBuffer() {
    LogToFile();
    m_logBuffer.clear();
  }

 private:
  static std::vector<std::string> m_logBuffer;
  static void LogToFile();
};
