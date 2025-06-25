#pragma once
#include <fstream>
#include <string>
#include <vector>

namespace ShaderLoader {

inline std::string LoadGLSL(const std::string& path) {
  std::ifstream file(path);
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

inline std::vector<uint32_t> LoadSPIRV(const std::string& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  // 检查文件是否成功打开
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open SPIRV file: " + path);
  }
  // 获取文件大小
  std::streampos fileSize = file.tellg();
  if (fileSize == -1) {
    throw std::runtime_error("Failed to get file size: " + path);
  }
  size_t size = static_cast<size_t>(fileSize);
  std::vector<uint32_t> result(size / sizeof(uint32_t));
  file.seekg(0);
  file.read(reinterpret_cast<char*>(result.data()), size);
  return result;
}

}  // namespace ShaderLoader
