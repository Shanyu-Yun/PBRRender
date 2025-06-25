#include "platform/vulkan/VKContext.hpp"
void VKContext::Init() {
  createInstance();
  selectPhysicalDevice();
  createSurface();
  createDevice();
  createSwapChain();
  createDescriptorPool();
  createCommandPools();
}

void VKContext::createInstance() {
  VKInstance::CreateInfo createInfo;
  createInfo.appName = "PBRRender";
  createInfo.appVersion = VK_MAKE_VERSION(1, 3, 0);
  createInfo.validationLayers = {"VK_LAYER_KHRONOS_validation"};
  m_instance = std::make_shared<VKInstance>(createInfo);
  Log::LogMessage(Log::Level::Info, "Vulkan instance created successfully.");

  m_surface = m_windowHandle->createVulkanSurface(m_instance->GetHandle());
  if (!m_surface) {
    Log::LogMessage(Log::Level::Error, "Failed to create Vulkan surface.");
  }
}

void VKContext::selectPhysicalDevice() {
  const auto& devices = m_instance->GetHandle().enumeratePhysicalDevices();
  for (const auto& device : devices) {
    if (device.getProperties().deviceType ==
        vk::PhysicalDeviceType::eDiscreteGpu) {
      m_physicalDevice = device;
      break;
    }
  }
}

void VKContext::createSurface() {
  m_surface = m_windowHandle->createVulkanSurface(m_instance->GetHandle());
  if (!m_surface) {
    Log::LogMessage(Log::Level::Error, "Failed to create Vulkan surface.");
    return;
  }
  Log::LogMessage(Log::Level::Info, "Vulkan surface created successfully.");
}

void VKContext::createDevice() {
  m_device = std::make_shared<VKDevice>(m_instance->GetHandle(), m_surface,
                                        m_physicalDevice);

  // 添加必需的设备扩展
  m_device->addRequiredExtension("VK_KHR_swapchain");

  m_device->createLogicalDevice();
  m_device->getQueue();
  Log::LogMessage(Log::Level::Info,
                  "Vulkan logical device created successfully.");
}

void VKContext::createSwapChain() {
  VKSwapChain::Config config;
  config.graphicsQueueFamilyIndex =
      m_device->m_queueFamilyIndices.graphicQueue.value();
  config.presentQueueFamilyIndex =
      m_device->m_queueFamilyIndices.presentQueue.value();
  config.preferredFormat = vk::Format::eB8G8R8A8Srgb;
  config.preferredColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
  config.preferredPresentMode = vk::PresentModeKHR::eMailbox;

  vk::Extent2D windowExtent =
      vk::Extent2D(m_windowHandle->getWidth(), m_windowHandle->getHeight());
  m_swapChain = std::make_shared<VKSwapChain>(
      m_device->GetHandle(), m_physicalDevice, m_surface, windowExtent, config);
  Log::LogMessage(Log::Level::Info, "Swapchain created successfully");
}

void VKContext::createDescriptorPool() {
  m_descriptorPool = std::make_shared<VKDescriptorPool>(m_device->GetHandle());
}

void VKContext::createCommandPools() {
  vk::CommandPoolCreateInfo poolInfo;

  //创建图形池
  poolInfo.queueFamilyIndex =
      m_device->m_queueFamilyIndices.graphicQueue.value();
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  m_graphicsCommandPool = std::make_shared<vk::CommandPool>(
      m_device->GetHandle().createCommandPool(poolInfo));

  Log::LogMessage(Log::Level::Info,
                  "Graphics command pool created successfully.");
  //创建传输池
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
  poolInfo.queueFamilyIndex =
      m_device->m_queueFamilyIndices.transferFamily.value();
  m_transferCommandPool = std::make_shared<vk::CommandPool>(
      m_device->GetHandle().createCommandPool(poolInfo));

  Log::LogMessage(Log::Level::Info,
                  "Transfer command pool created successfully.");
  //创建计算池
  poolInfo.queueFamilyIndex =
      m_device->m_queueFamilyIndices.computeFamily.value();
  m_computeCommandPool = std::make_shared<vk::CommandPool>(
      m_device->GetHandle().createCommandPool(poolInfo));
  Log::LogMessage(Log::Level::Info,
                  "Compute command pool created successfully.");
}