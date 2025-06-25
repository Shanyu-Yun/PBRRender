#pragma once
#include <memory>

#include "VKContext.hpp"
#include "VKShader.hpp"
#include "core/interface/IRenderer.hpp"
#include "vkbasic/VKDescriptorPool.hpp"
#include "vkbasic/VKDevice.hpp"
#include "vkbasic/VKInstance.hpp"
#include "vkbasic/VKSwapChain.hpp"

class VKRender : public IRenderer {
 public:
  VKRender() = default;
  ~VKRender(){};

  bool init(Window* windowHandle) override;
  void resize(int width, int height) override;
  void renderFrame() override;

  void setModel(const Model& model) override;
  void setMaterial(const Material& material) override;
  void setCamera() override;

 private:
  uint32_t m_currentFrame = 0;
  static const int MAX_FRAMES_IN_FLIGHT = 2;

  //上下文
  std::shared_ptr<VKContext> m_vkContext;

  // 渲染资源
  std::vector<vk::Image> m_textureImage;
  std::vector<vk::DeviceMemory> m_textureImageMemory;
  std::vector<vk::ImageView> m_textureImageView;
  vk::Buffer m_vertexBuffer;
  vk::DeviceMemory m_vertexBufferMemory;
  vk::Buffer m_indexBuffer;
  vk::DeviceMemory m_indexBufferMemory;

  Model m_currentModel;
  Material m_currentMaterial;
  void uploadModelData();
  void uploadMaterialData();
  void createTextureImage(Texture& T);
  void createTextureImageView();
};