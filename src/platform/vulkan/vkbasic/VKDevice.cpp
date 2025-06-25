#include "platform/vulkan/vkbasic/VKDevice.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

VKDevice::~VKDevice() { m_device.destroy(); }

void VKDevice::getQueue() {
  m_device.getQueue(m_queueFamilyIndices.graphicQueue.value(), 0,
                    &m_graphicsQueue);
  m_device.getQueue(m_queueFamilyIndices.presentQueue.value(), 0,
                    &m_presentQueue);

  // 传输队列：如果与图形队列相同，则直接使用图形队列
  if (m_queueFamilyIndices.transferFamily.value() ==
      m_queueFamilyIndices.graphicQueue.value()) {
    m_transferQueue = m_graphicsQueue;
  } else {
    // 只有当传输队列族实际被创建时才获取
    m_device.getQueue(m_queueFamilyIndices.transferFamily.value(), 0,
                      &m_transferQueue);
  }

  // 计算队列：如果与图形队列相同，则直接使用图形队列
  if (m_queueFamilyIndices.computeFamily.value() ==
      m_queueFamilyIndices.graphicQueue.value()) {
    m_computeQueue = m_graphicsQueue;
  } else {
    // 只有当计算队列族实际被创建时才获取
    m_device.getQueue(m_queueFamilyIndices.computeFamily.value(), 0,
                      &m_computeQueue);
  }
}

void VKDevice::findQueueIndices() {
  // 获取队列族属性
  auto queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

  for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
    // 查找图形队列族
    if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      m_queueFamilyIndices.graphicQueue = i;
    }

    // 查找呈现队列族
    if (m_physicalDevice.getSurfaceSupportKHR(i, m_surface)) {
      m_queueFamilyIndices.presentQueue = i;
    }

    // 查找专门的传输队列族（只支持传输，不支持图形）
    if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer) &&
        !(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
        !(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute)) {
      m_queueFamilyIndices.transferFamily = i;
    }

    // 查找专门的计算队列族（支持计算，但不支持图形）
    if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) &&
        !(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)) {
      m_queueFamilyIndices.computeFamily = i;
    }
  }

  // 如果没有找到专门的传输队列族，使用图形队列族
  if (!m_queueFamilyIndices.transferFamily.has_value()) {
    m_queueFamilyIndices.transferFamily = m_queueFamilyIndices.graphicQueue;
  }

  // 如果没有找到专门的计算队列族，使用图形队列族
  if (!m_queueFamilyIndices.computeFamily.has_value()) {
    m_queueFamilyIndices.computeFamily = m_queueFamilyIndices.graphicQueue;
  }
}

void VKDevice::queryAvailableExtensions() {
  m_availableExtensions = m_physicalDevice.enumerateDeviceExtensionProperties();
}

bool VKDevice::isExtensionSupported(const std::string& extensionName) const {
  return std::any_of(
      m_availableExtensions.begin(), m_availableExtensions.end(),
      [&extensionName](const vk::ExtensionProperties& extension) {
        return extensionName == extension.extensionName.data();
      });
}

void VKDevice::addRequiredExtension(const std::string& extensionName) {
  if (std::find(m_requiredExtensions.begin(), m_requiredExtensions.end(),
                extensionName) == m_requiredExtensions.end()) {
    m_requiredExtensions.push_back(extensionName);
  }
}

void VKDevice::addRequiredExtensions(
    const std::vector<std::string>& extensionNames) {
  for (const auto& extensionName : extensionNames) {
    addRequiredExtension(extensionName);
  }
}

bool VKDevice::validateRequiredExtensions() const {
  for (const auto& requiredExtension : m_requiredExtensions) {
    if (!isExtensionSupported(requiredExtension)) {
      std::cerr << "Required extension not supported: " << requiredExtension
                << std::endl;
      return false;
    }
  }
  return true;
}

std::vector<const char*> VKDevice::getRequiredExtensionNames() const {
  std::vector<const char*> extensionNames;
  extensionNames.reserve(m_requiredExtensions.size());
  for (const auto& extension : m_requiredExtensions) {
    extensionNames.push_back(extension.c_str());
  }
  return extensionNames;
}

void VKDevice::printAvailableExtensions() const {
  std::cout << "Available device extensions:\n";
  for (const auto& extension : m_availableExtensions) {
    std::cout << "  " << extension.extensionName << " (version "
              << extension.specVersion << ")\n";
  }
}

void VKDevice::printEnabledExtensions() const {
  std::cout << "Enabled device extensions:\n";
  for (const auto& extension : m_enabledExtensions) {
    std::cout << "  " << extension << "\n";
  }
}

void VKDevice::createLogicalDevice(const std::vector<const char*>& extensions) {
  // 创建逻辑设备
  //需要注意图形队列和呈现队列可能不是同一个队列
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  // 收集所有需要的队列族
  std::set<uint32_t> uniqueQueueFamilies = {
      m_queueFamilyIndices.graphicQueue.value(),
      m_queueFamilyIndices.presentQueue.value()};

  // 只有当传输队列族与图形队列族不同时才添加
  if (m_queueFamilyIndices.transferFamily.has_value() &&
      m_queueFamilyIndices.transferFamily.value() !=
          m_queueFamilyIndices.graphicQueue.value()) {
    uniqueQueueFamilies.insert(m_queueFamilyIndices.transferFamily.value());
  }

  // 只有当计算队列族与图形队列族不同时才添加
  if (m_queueFamilyIndices.computeFamily.has_value() &&
      m_queueFamilyIndices.computeFamily.value() !=
          m_queueFamilyIndices.graphicQueue.value()) {
    uniqueQueueFamilies.insert(m_queueFamilyIndices.computeFamily.value());
  }

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(queueFamily)
        .setQueueCount(1)
        .setPQueuePriorities(&queuePriority);
    queueCreateInfos.push_back(queueCreateInfo);
  }

  vk::PhysicalDeviceFeatures deviceFeatures;
  deviceFeatures.fillModeNonSolid = VK_TRUE;  // 启用非实心填充模式特性
  deviceFeatures.samplerAnisotropy = VK_TRUE;  // 启用各向异性过滤特性

  vk::DeviceCreateInfo createInfo;
  createInfo
      .setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()))
      .setPQueueCreateInfos(queueCreateInfos.data());
  createInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
      .setPpEnabledExtensionNames(extensions.data());
  createInfo.setPEnabledFeatures(&deviceFeatures);

  // 记录启用的扩展
  m_enabledExtensions.clear();
  for (const char* extension : extensions) {
    m_enabledExtensions.insert(std::string(extension));
  }

  m_device = m_physicalDevice.createDevice(createInfo);
}

void VKDevice::createLogicalDevice() {
  if (!validateRequiredExtensions()) {
    throw std::runtime_error(
        "Required extensions are not supported by the physical device!");
  }

  auto extensionNames = getRequiredExtensionNames();
  createLogicalDevice(extensionNames);
}

vk::Format VKDevice::findSupportedFormat(
    const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
    vk::FormatFeatureFlags features) {
  for (vk::Format format : candidates) {
    vk::FormatProperties props = m_physicalDevice.getFormatProperties(format);

    if ((tiling == vk::ImageTiling::eLinear &&
         (props.linearTilingFeatures & features) == features) ||
        (tiling == vk::ImageTiling::eOptimal &&
         (props.optimalTilingFeatures & features) == features)) {
      return format;
    }
  }
  throw std::runtime_error("Failed to find a supported format!");
}
