#pragma once
#include <string>
#include <vulkan/vulkan.hpp>

class VKShader {
 public:
  std::string name;
  VKShader(const std::string& name, const std::string& vertexPath,
           const std::string& fragmentPath);
  ~VKShader();

  vk::ShaderModule GetVertexModule() const;
  vk::ShaderModule GetFragmentModule() const;

 private:
  vk::Device m_device;
  vk::ShaderModule m_vertexModule;
  vk::ShaderModule m_fragmentModule;

  vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code);
};
