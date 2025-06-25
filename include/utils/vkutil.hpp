#pragma once
#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "resource/Texture.hpp"


namespace vkutil {
/**
 * @brief 将纹理通道类型转换为Vulkan格式
 * @param channelType 纹理的通道类型
 * @param isSRGB 是否使用sRGB格式 (默认: true，用于颜色纹理)
 * @return 对应的Vulkan格式
 */
inline vk::Format ChannelTypeToVkFormat(ChannelType channelType,
                                        bool isSRGB = true) {
  switch (channelType) {
    case ChannelType::R:
      return isSRGB ? vk::Format::eR8Srgb : vk::Format::eR8Unorm;

    case ChannelType::RG:
      return isSRGB ? vk::Format::eR8G8Srgb : vk::Format::eR8G8Unorm;

    case ChannelType::RGB:
      return isSRGB ? vk::Format::eR8G8B8Srgb : vk::Format::eR8G8B8Unorm;

    case ChannelType::RGBA:
      return isSRGB ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm;

    case ChannelType::BGR:
      return isSRGB ? vk::Format::eB8G8R8Srgb : vk::Format::eB8G8R8Unorm;

    case ChannelType::BGRA:
      return isSRGB ? vk::Format::eB8G8R8A8Srgb : vk::Format::eB8G8R8A8Unorm;

    case ChannelType::None:
    default:  // 默认使用RGBA格式
      return isSRGB ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm;
  }
}
/**
 * @brief 根据纹理类型将纹理转换为合适的Vulkan格式
 * @param texture 纹理对象
 * @return 对应的Vulkan格式
 */
inline vk::Format TextureToVkFormat(const Texture& texture) {
  bool useSRGB = true;

  // 线性格式优先的特殊情况
  switch (texture.type) {
    case TextureType::Normal:
    case TextureType::Roughness:
    case TextureType::Metallic:
    case TextureType::AmbientOcclusion:
    case TextureType::HeightMap:
      useSRGB = false;
      break;
    default:
      useSRGB = true;
      break;
  }

  return ChannelTypeToVkFormat(texture.channelType, useSRGB);
}

/**
 * @brief 查找合适的内存类型索引
 * @param physicalDevice Vulkan物理设备
 * @param typeFilter 内存类型位掩码
 * @param properties 所需的内存属性标志
 * @return 内存类型索引
 */
inline uint32_t FindMemoryType(vk::PhysicalDevice physicalDevice,
                               uint32_t typeFilter,
                               vk::MemoryPropertyFlags properties) {
  vk::PhysicalDeviceMemoryProperties memProperties =
      physicalDevice.getMemoryProperties();

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("无法找到合适的内存类型!");
}

}  // namespace vkutil