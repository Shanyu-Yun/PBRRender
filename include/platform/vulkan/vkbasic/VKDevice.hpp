#pragma once
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

/**
 * @brief Vulkan逻辑设备管理类
 *
 * 负责管理Vulkan物理设备和逻辑设备，提供设备选择和功能查询。
 * 处理队列族查找和队列创建，支持图形和呈现队列。
 * 管理设备扩展和特性启用。
 * 提供逻辑设备句柄访问和设备相关资源的创建方法。
 */
class VKDevice {
 public:
  // 定义结构体，存储队列信息
  struct QueueFamilyIndices final {
    std::optional<uint32_t> graphicQueue;
    std::optional<uint32_t> presentQueue;
    std::optional<uint32_t> transferFamily;  // 专门的传输队列族
    std::optional<uint32_t> computeFamily;   // 计算队列族
  };

  const vk::Instance m_instance;
  const vk::SurfaceKHR m_surface;
  vk::Device m_device;
  vk::PhysicalDevice m_physicalDevice;

  vk::Queue m_graphicsQueue;
  vk::Queue m_presentQueue;
  vk::Queue m_transferQueue;
  vk::Queue m_computeQueue;
  QueueFamilyIndices m_queueFamilyIndices;

  // 扩展管理相关成员
  std::vector<std::string> m_requiredExtensions;
  std::vector<vk::ExtensionProperties> m_availableExtensions;
  std::set<std::string> m_enabledExtensions;

  VKDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface,
           const vk::PhysicalDevice& physicalDevice)
      : m_instance(instance),
        m_surface(surface),
        m_physicalDevice(physicalDevice) {
    findQueueIndices();
    queryAvailableExtensions();
  }
  ~VKDevice();

  // 队列访问方法
  vk::Queue& GetGraphicsQueue() { return m_graphicsQueue; }
  vk::Queue& GetPresentQueue() { return m_presentQueue; }
  vk::Queue& GetTransferQueue() { return m_transferQueue; }
  vk::Queue& GetComputeQueue() { return m_computeQueue; }

  // 队列相关方法
  void getQueue();
  void findQueueIndices();

  // 设备创建相关方法
  void createLogicalDevice(const std::vector<const char*>& extensions);
  void createLogicalDevice();  // 使用内部管理的扩展列表

  // 扩展管理方法
  void queryAvailableExtensions();
  bool isExtensionSupported(const std::string& extensionName) const;
  void addRequiredExtension(const std::string& extensionName);
  void addRequiredExtensions(const std::vector<std::string>& extensionNames);
  bool validateRequiredExtensions() const;
  std::vector<const char*> getRequiredExtensionNames() const;
  void printAvailableExtensions() const;
  void printEnabledExtensions() const;

  // 设备操作方法
  void waitIdle() { m_device.waitIdle(); }
  vk::Device& GetHandle() { return m_device; }

  // 格式查找方法
  vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates,
                                 vk::ImageTiling tiling,
                                 vk::FormatFeatureFlags features);
};