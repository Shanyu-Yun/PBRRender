#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../WindowHandler.hpp"
#include "VKShader.hpp"
#include "vkbasic/VKDescriptorPool.hpp"
#include "vkbasic/VKDevice.hpp"
#include "vkbasic/VKInstance.hpp"
#include "vkbasic/VKSwapChain.hpp"

class VKContext {
 public:
  VKContext(Window* window) : m_windowHandle(window) { Init(); }
  Window* m_windowHandle;

  // 渲染管线
  vk::PipelineLayout m_pipelineLayout;
  vk::Pipeline m_graphicsPipeline;

  // 描述符相关
  vk::DescriptorSetLayout m_descriptorSetLayout;
  std::vector<vk::DescriptorSet> m_descriptorSets;
  vk::Sampler m_textureSampler;

  // 同步对象
  std::vector<vk::Semaphore> m_imageAvailableSemaphores;
  std::vector<vk::Semaphore> m_renderFinishedSemaphores;
  std::vector<vk::Fence> m_inFlightFences;
  std::vector<vk::CommandBuffer> m_commandBuffers;
  //基础vulkan类
  std::shared_ptr<VKInstance> m_instance;
  std::shared_ptr<VKShader> m_shader;
  std::shared_ptr<VKDevice> m_device;
  std::shared_ptr<VKDescriptorPool> m_descriptorPool;
  std::shared_ptr<VKSwapChain> m_swapChain;
  vk::PhysicalDevice m_physicalDevice;
  vk::SurfaceKHR m_surface;
  //命令缓冲池
  std::shared_ptr<vk::CommandPool> m_graphicsCommandPool;
  std::shared_ptr<vk::CommandPool> m_transferCommandPool;
  std::shared_ptr<vk::CommandPool> m_computeCommandPool;

 private:
  void Init();
  void createInstance();
  void selectPhysicalDevice();
  void createSurface();
  void createDevice();
  void createSwapChain();
  void createDescriptorPool();
  void createCommandPools();
};