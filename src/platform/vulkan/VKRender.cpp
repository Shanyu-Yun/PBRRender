#include "platform/vulkan/VKRender.hpp"

#include "core/Log.hpp"
#include "utils/vkutil.hpp"

bool VKRender::init(Window* windowHandle) {
  m_windowHandle = windowHandle;
  m_vkContext = std::make_shared<VKContext>(windowHandle);
  if (!m_vkContext) {
    Log::LogMessage(Log::Level::Error, "Failed to create VKContext.");
    return false;
  }
  Log::LogMessage(Log::Level::Info, "VKContext created successfully.");
  return true;
}

void VKRender::resize(int width, int height) {}

void VKRender::renderFrame() {}

void VKRender::setModel(const Model& model) { m_currentModel = model; }

void VKRender::setMaterial(const Material& material) {
  m_currentMaterial = material;
}

void VKRender::setCamera() {}

void VKRender::uploadModelData() {}

void VKRender::uploadMaterialData() {}

void VKRender::createTextureImage(Texture& T) {
  vk::ImageCreateInfo imageCreateInfo;
  // 设置图像创建信息
  imageCreateInfo.imageType = vk::ImageType::e2D;
  imageCreateInfo.format = vkutil::TextureToVkFormat(T);
  imageCreateInfo.extent.width = T.width;    // 示例宽度
  imageCreateInfo.extent.height = T.height;  // 示例高度
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
  imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
  imageCreateInfo.usage =
      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;

  imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
  imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;

  m_vkContext->m_device->GetHandle().createImage(
      &imageCreateInfo, nullptr, &m_textureImage.emplace_back());

  vk::MemoryRequirements memRequirements;
  m_vkContext->m_device->GetHandle().getImageMemoryRequirements(
      m_textureImage.back(), &memRequirements);

  vk::MemoryAllocateInfo allocInfo;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = vkutil::FindMemoryType(
      m_vkContext->m_physicalDevice, memRequirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  m_vkContext->m_device->GetHandle().allocateMemory(
      &allocInfo, nullptr, &m_textureImageMemory.emplace_back());

  m_vkContext->m_device->GetHandle().bindImageMemory(
      m_textureImage.back(), m_textureImageMemory.back(), 0);

  vk::BufferCreateInfo bufferCreateInfo;
}

void VKRender::createTextureImageView() {}
