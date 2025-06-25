#include "platform/vulkan/vkbasic/VKInstance.hpp"

#include <vulkan/vulkan.h>  // 添加此行以确保宏定义可用

#include <stdexcept>

VKInstance::VKInstance(const CreateInfo& info) {
  // --- 1. 收集扩展 ---
  std::vector<const char*> extensions = GetRequiredPlatformExtensions();

  // 添加用户请求的扩展
  for (const auto& ext : info.requiredExtensions) {
    extensions.push_back(ext);
  }
  for (const auto& ext : info.optionalExtensions) {
    if (CheckExtensionSupport(ext)) {
      extensions.push_back(ext);
    }
  }

  // --- 2. 验证层检查 ---
  std::vector<const char*> layers;
  if (!info.validationLayers.empty()) {
    if (CheckValidationLayerSupport(info.validationLayers)) {
      layers = info.validationLayers;
    } else {
      throw std::runtime_error("Requested validation layers not available");
    }
  }

  // --- 3. 创建实例 ---
  vk::ApplicationInfo appInfo;
  appInfo.pApplicationName = info.appName.c_str();
  appInfo.applicationVersion = info.appVersion;
  appInfo.pEngineName = "NoEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  vk::InstanceCreateInfo createInfo;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();
  createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  createInfo.ppEnabledLayerNames = layers.data();

  try {
    m_instance = vk::createInstance(createInfo);
    m_enabledExtensions = extensions;
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to create Vulkan instance: " +
                             std::string(err.what()));
  }
}

VKInstance::~VKInstance() {
  if (m_instance) {
    m_instance.destroy();
  }
}

// --- 辅助函数 ---
std::vector<const char*> VKInstance::GetRequiredPlatformExtensions() {
  std::vector<const char*> extensions;

#if defined(_WIN32)
  extensions.push_back("VK_KHR_win32_surface");
#elif defined(__linux__)
  extensions.push_back("VK_KHR_xlib_surface");
#elif defined(__APPLE__)
  extensions.push_back("VK_EXT_metal_surface");
#endif

  extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  return extensions;
}

bool VKInstance::CheckValidationLayerSupport(
    const std::vector<const char*>& layers) {
  auto availableLayers = vk::enumerateInstanceLayerProperties();
  for (const char* layer : layers) {
    bool found = false;
    for (const auto& availLayer : availableLayers) {
      if (strcmp(layer, availLayer.layerName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  return true;
}

bool VKInstance::CheckExtensionSupport(const char* extension) {
  auto availableExtensions = vk::enumerateInstanceExtensionProperties();
  for (const auto& ext : availableExtensions) {
    if (strcmp(extension, ext.extensionName) == 0) {
      return true;
    }
  }
  return false;
}