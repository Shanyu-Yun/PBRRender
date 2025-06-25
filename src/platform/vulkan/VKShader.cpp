#include "platform/vulkan/VKShader.hpp"

#include "utils/ShaderLoader.hpp"

VKShader::VKShader(const std::string& shaderName, const std::string& vertexPath,
                   const std::string& fragmentPath)
    : name(shaderName) {
  // 加载着色器代码
  auto vertexCode = ShaderLoader::LoadSPIRV(vertexPath);
  auto fragmentCode = ShaderLoader::LoadSPIRV(fragmentPath);

  // 创建着色器模块
  m_vertexModule = CreateShaderModule(vertexCode);
  m_fragmentModule = CreateShaderModule(fragmentCode);
}

VKShader::~VKShader() {
  if (m_vertexModule) {
    m_device.destroyShaderModule(m_vertexModule);
  }
  if (m_fragmentModule) {
    m_device.destroyShaderModule(m_fragmentModule);
  }
}

vk::ShaderModule VKShader::GetVertexModule() const { return m_vertexModule; }

vk::ShaderModule VKShader::GetFragmentModule() const {
  return m_fragmentModule;
}

vk::ShaderModule VKShader::CreateShaderModule(
    const std::vector<uint32_t>& code) {
  vk::ShaderModuleCreateInfo createInfo;
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  try {
    return m_device.createShaderModule(createInfo);
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to create shader module: " +
                             std::string(err.what()));
  }
}