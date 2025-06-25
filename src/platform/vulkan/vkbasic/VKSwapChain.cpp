#include "platform/vulkan/vkbasic/VKSwapChain.hpp"

VKSwapChain::VKSwapChain(vk::Device device, vk::PhysicalDevice physicalDevice,
                         vk::SurfaceKHR surface,
                         const vk::Extent2D& windowExtent, const Config& config)
    : m_device(device),
      m_physicalDevice(physicalDevice),
      m_surface(surface),
      m_windowExtent(windowExtent),
      m_config(config) {
  CreateSwapchain();
  CreateImageViews();
}

VKSwapChain::~VKSwapChain() { Cleanup(); }

void VKSwapChain::CreateSwapchain() {
  SupportDetails support = QuerySwapchainSupport();
  vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
  vk::PresentModeKHR presentMode = ChooseSwapPresentMode();
  vk::Extent2D extent = ChooseSwapExtent(support.capabilities);

  uint32_t imageCount =
      std::clamp(m_config.minImageCount, support.capabilities.minImageCount,
                 (support.capabilities.maxImageCount > 0)
                     ? support.capabilities.maxImageCount
                     : std::numeric_limits<uint32_t>::max());

  vk::SwapchainCreateInfoKHR createInfo;
  createInfo.surface = m_surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = m_config.imageUsage;
  createInfo.preTransform = support.capabilities.currentTransform;
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  uint32_t queueFamilyIndices[] = {m_config.graphicsQueueFamilyIndex,
                                   m_config.presentQueueFamilyIndex};
  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  try {
    m_swapchain = m_device.createSwapchainKHR(createInfo);
    m_swapchainImages = m_device.getSwapchainImagesKHR(m_swapchain);
    m_swapchainImageFormat = surfaceFormat.format;
    m_swapchainExtent = extent;
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to create swapchain: " +
                             std::string(err.what()));
  }
}

void VKSwapChain::CreateImageViews() {
  m_swapchainImageViews.resize(m_swapchainImages.size());
  for (size_t i = 0; i < m_swapchainImages.size(); i++) {
    vk::ImageViewCreateInfo createInfo;
    createInfo.image = m_swapchainImages[i];
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = m_swapchainImageFormat;
    createInfo.components = {
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity};
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    try {
      m_swapchainImageViews[i] = m_device.createImageView(createInfo);
    } catch (const vk::SystemError& err) {
      throw std::runtime_error("Failed to create image views: " +
                               std::string(err.what()));
    }
  }
}

void VKSwapChain::Cleanup() {
  for (auto imageView : m_swapchainImageViews) {
    m_device.destroyImageView(imageView);
  }
  if (m_swapchain) {
    m_device.destroySwapchainKHR(m_swapchain);
  }
}

VKSwapChain::SupportDetails VKSwapChain::QuerySwapchainSupport() const {
  SupportDetails details;
  details.capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
  details.formats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);
  details.presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
  return details;
}

vk::SurfaceFormatKHR VKSwapChain::ChooseSwapSurfaceFormat() const {
  for (const auto& format : QuerySwapchainSupport().formats) {
    if (format.format == m_config.preferredFormat &&
        format.colorSpace == m_config.preferredColorSpace) {
      return format;
    }
  }
  return QuerySwapchainSupport().formats[0];
}

vk::PresentModeKHR VKSwapChain::ChooseSwapPresentMode() const {
  if (m_config.enableVSync) {
    return vk::PresentModeKHR::eFifo;  // 强制垂直同步
  }

  for (const auto& mode : QuerySwapchainSupport().presentModes) {
    if (mode == m_config.preferredPresentMode) {
      return mode;
    }
  }
  return vk::PresentModeKHR::eFifo;  // 保证存在的默认模式
}

vk::Extent2D VKSwapChain::ChooseSwapExtent(
    const vk::SurfaceCapabilitiesKHR& capabilities) const {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    return {
        std::clamp(m_windowExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width),
        std::clamp(m_windowExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height)};
  }
}

void VKSwapChain::Recreate(const vk::Extent2D& newExtent) {
  m_windowExtent = newExtent;
  Cleanup();
  CreateSwapchain();
  CreateImageViews();
}

vk::Result VKSwapChain::AcquireNextImage(
    vk::Semaphore imageAvailableSemaphore) {
  try {
    vk::Result result = m_device.acquireNextImageKHR(
        m_swapchain, std::numeric_limits<uint64_t>::max(),
        imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);
    return result;
  } catch (const vk::OutOfDateKHRError&) {
    Recreate(m_windowExtent);
    return AcquireNextImage(imageAvailableSemaphore);  // 递归重试
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to acquire swapchain image: " +
                             std::string(err.what()));
  }
}

void VKSwapChain::Present(vk::Queue presentQueue, uint32_t imageIndex,
                          vk::Semaphore renderFinishedSemaphore) {
  vk::PresentInfoKHR presentInfo;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &m_swapchain;
  presentInfo.pImageIndices = &imageIndex;

  try {
    presentQueue.presentKHR(presentInfo);
  } catch (const vk::OutOfDateKHRError&) {
    Recreate(m_windowExtent);
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to present swapchain image: " +
                             std::string(err.what()));
  }
}