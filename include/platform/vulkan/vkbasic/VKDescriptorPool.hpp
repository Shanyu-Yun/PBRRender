#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

/**
 * @brief Vulkan描述符池管理类
 *
 * 负责创建和管理Vulkan描述符池，支持动态分配描述符集。
 * 提供不同类型描述符的比例配置，优化内存使用。
 * 支持描述符池资源的自动管理和回收。
 * 实现缓存机制，提高描述符集分配效率。
 */
class VKDescriptorPool {
 public:
  struct PoolSizeRatio {
    vk::DescriptorType type;
    float ratio;  // 该类型描述符占总池的比例
  };

  struct Config {
    uint32_t maxSets = 1000;                   // 池中最大描述符集数量
    std::vector<PoolSizeRatio> sizeRatios = {  // 默认比例配置
        {vk::DescriptorType::eUniformBuffer, 0.5f},
        {vk::DescriptorType::eCombinedImageSampler, 0.3f},
        {vk::DescriptorType::eStorageBuffer, 0.2f}};
    vk::DescriptorPoolCreateFlags flags =
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    bool allowAutoExpand = true;  // 池满时自动扩容
    bool threadSafe = false;
  };

  VKDescriptorPool(vk::Device device, const Config& config = {});
  ~VKDescriptorPool();

  // 分配描述符集
  vk::DescriptorSet AllocateSet(vk::DescriptorSetLayout layout);

  // 批量分配
  std::vector<vk::DescriptorSet> AllocateSets(vk::DescriptorSetLayout layout,
                                              uint32_t count);
  std::vector<vk::DescriptorSet> AllocateSets(
      const std::vector<vk::DescriptorSetLayout>& layouts);

  // 释放单个描述符集（需启用 eFreeDescriptorSet 标志）
  void FreeSet(vk::DescriptorSet set);

  // 重置整个池
  void Reset();

 private:
  struct InternalPool {
    vk::DescriptorPool pool;
    uint32_t allocatedSets = 0;
  };

  void CreateNewPool();
  void UpdatePoolStats();

  vk::Device m_device;
  Config m_config;
  std::vector<InternalPool> m_pools;  // 池集合（支持自动扩容）
  std::unordered_map<vk::DescriptorType, uint32_t>
      m_typeCounts;  // 类型计数缓存
};