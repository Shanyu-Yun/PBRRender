#pragma once
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>

/**
 * @brief Vulkan交换链管理类
 *
 * 负责创建和管理Vulkan交换链，处理窗口表面与显示设备的交互。
 * 提供帧缓冲管理、图像获取和呈现等功能。
 * 支持窗口大小变化时的交换链重建。
 * 处理不同显示设备的表面格式和呈现模式选择。
 */
class VKSwapChain {
 public:
  struct SupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
  };

  struct Config {
    uint32_t minImageCount = 2;  // 最小缓冲数量
    vk::Format preferredFormat = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR preferredColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR preferredPresentMode = vk::PresentModeKHR::eMailbox;
    vk::ImageUsageFlags imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    bool enableVSync = false;               // 是否强制垂直同步
    uint32_t graphicsQueueFamilyIndex = 0;  // 图形队列族索引
    uint32_t presentQueueFamilyIndex = 1;   // 呈现队列族索引
  };

  VKSwapChain(vk::Device device, vk::PhysicalDevice physicalDevice,
              vk::SurfaceKHR surface, const vk::Extent2D& windowExtent,
              const Config& config = {});

  ~VKSwapChain();

  // 禁止拷贝
  VKSwapChain(const VKSwapChain&) = delete;
  VKSwapChain& operator=(const VKSwapChain&) = delete;

  // 重建交换链（窗口大小变化时调用）
  void Recreate(const vk::Extent2D& newExtent);

  // 获取下一帧图像
  vk::Result AcquireNextImage(vk::Semaphore imageAvailableSemaphore);

  // 提交呈现命令
  void Present(vk::Queue presentQueue, uint32_t imageIndex,
               vk::Semaphore renderFinishedSemaphore);

  // 访问器方法
  vk::Format getImageFormat() const { return m_swapchainImageFormat; }
  vk::Extent2D GetExtent() const { return m_swapchainExtent; }
  const std::vector<vk::ImageView>& GetImageViews() const {
    return m_swapchainImageViews;
  }
  const std::vector<vk::Image>& GetImages() const {
    return m_swapchainImages;
  }
  vk::Image GetImage(uint32_t index) const {
    return m_swapchainImages[index];
  }
  size_t GetImageCount() const { return m_swapchainImages.size(); }
  vk::SwapchainKHR& GetHandle() { return m_swapchain; }

  uint32_t CurrentImageIndex() { return m_currentImageIndex; }

 private:
  void CreateSwapchain();
  void CreateImageViews();
  void Cleanup();

  // 辅助函数
  SupportDetails QuerySwapchainSupport() const;
  vk::SurfaceFormatKHR ChooseSwapSurfaceFormat() const;
  vk::PresentModeKHR ChooseSwapPresentMode() const;
  vk::Extent2D ChooseSwapExtent(
      const vk::SurfaceCapabilitiesKHR& capabilities) const;

  // Vulkan对象
  vk::Device m_device;
  vk::PhysicalDevice m_physicalDevice;
  vk::SurfaceKHR m_surface;
  vk::Extent2D m_windowExtent;

  // 交换链相关资源
  uint32_t m_currentImageIndex = 0;
  vk::SwapchainKHR m_swapchain;
  vk::Format m_swapchainImageFormat;
  vk::Extent2D m_swapchainExtent;
  std::vector<vk::Image> m_swapchainImages;
  std::vector<vk::ImageView> m_swapchainImageViews;

  // 配置参数
  Config m_config;
};