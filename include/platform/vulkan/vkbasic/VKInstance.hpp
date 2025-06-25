#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

/**
 * @brief Vulkan实例管理类
 *
 * 负责创建和管理Vulkan实例，处理应用程序与Vulkan运行时的交互。
 * 支持验证层和扩展配置，提高开发调试效率。
 * 提供物理设备枚举和选择功能。
 * 管理实例级别的资源和回调，如调试消息回调。
 */
class VKInstance {
 public:
  struct CreateInfo {
    std::string appName = "VulkanApp";
    uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
    std::vector<const char*> requiredExtensions = {};  // 必须启用的扩展
    std::vector<const char*> optionalExtensions =
        {};  // 可选扩展（自动跳过不支持的）
    std::vector<const char*> validationLayers = {};  // 留空则禁用验证层
  };

  explicit VKInstance(const CreateInfo& info = {});
  ~VKInstance();

  // 禁止拷贝
  VKInstance(const VKInstance&) = delete;
  VKInstance& operator=(const VKInstance&) = delete;

  vk::Instance GetHandle() const { return m_instance; }
  const std::vector<const char*>& GetEnabledExtensions() const {
    return m_enabledExtensions;
  }

 private:
  static std::vector<const char*> GetRequiredPlatformExtensions();
  bool CheckExtensionSupport(const char* extensionName);
  bool CheckValidationLayerSupport(
      const std::vector<const char*>& validationLayers);

  vk::Instance m_instance;
  std::vector<const char*> m_enabledExtensions;
};