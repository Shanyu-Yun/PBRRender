#include "platform/vulkan/vkbasic/VKDescriptorPool.hpp"

VKDescriptorPool::VKDescriptorPool(vk::Device device, const Config& config)
    : m_device(device), m_config(config) {
  UpdatePoolStats();
  CreateNewPool();  // 创建第一个池
}

VKDescriptorPool::~VKDescriptorPool() {
  for (auto& pool : m_pools) {
    m_device.destroyDescriptorPool(pool.pool);
  }
}

void VKDescriptorPool::CreateNewPool() {
  std::vector<vk::DescriptorPoolSize> poolSizes;
  for (const auto& [type, count] : m_typeCounts) {
    poolSizes.push_back({type, count});
  }

  vk::DescriptorPoolCreateInfo createInfo;
  createInfo.flags = m_config.flags;
  createInfo.maxSets = m_config.maxSets;
  createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  createInfo.pPoolSizes = poolSizes.data();

  try {
    InternalPool newPool;
    newPool.pool = m_device.createDescriptorPool(createInfo);
    m_pools.push_back(newPool);
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to create descriptor pool: " +
                             std::string(err.what()));
  }
}

void VKDescriptorPool::UpdatePoolStats() {
  m_typeCounts.clear();
  for (const auto& ratio : m_config.sizeRatios) {
    m_typeCounts[ratio.type] = static_cast<uint32_t>(
        ratio.ratio * m_config.maxSets * 10  // 乘以基数保证最小数量
    );
  }
}

vk::DescriptorSet VKDescriptorPool::AllocateSet(
    vk::DescriptorSetLayout layout) {
  for (auto& pool : m_pools) {
    if (pool.allocatedSets < m_config.maxSets) {
      try {
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.descriptorPool = pool.pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        auto sets = m_device.allocateDescriptorSets(allocInfo);
        pool.allocatedSets++;
        return sets[0];
      } catch (const vk::OutOfPoolMemoryError& err) {
        // 专门捕获池内存不足错误
        if (m_config.allowAutoExpand) {
          CreateNewPool();
          continue;
        }
        throw std::runtime_error("Descriptor pool out of memory");
      } catch (const vk::SystemError& err) {
        // 处理其他Vulkan错误
        throw std::runtime_error("Descriptor set allocation failed: " +
                                 std::string(err.what()));
      }
    }
  }
  throw std::runtime_error("All descriptor pools exhausted");
}

std::vector<vk::DescriptorSet> VKDescriptorPool::AllocateSets(
    const std::vector<vk::DescriptorSetLayout>& layouts) {
  std::vector<vk::DescriptorSet> sets;
  sets.reserve(layouts.size());
  for (const auto& layout : layouts) {
    sets.push_back(AllocateSet(layout));
  }
  return sets;
}

std::vector<vk::DescriptorSet> VKDescriptorPool::AllocateSets(
    vk::DescriptorSetLayout layout, uint32_t count) {
  std::vector<vk::DescriptorSetLayout> layouts(count, layout);
  return AllocateSets(layouts);
}

void VKDescriptorPool::FreeSet(vk::DescriptorSet set) {
  if (!(m_config.flags &
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)) {
    throw std::logic_error("Pool not created with eFreeDescriptorSet flag");
  }

  for (auto& pool : m_pools) {
    try {
      m_device.freeDescriptorSets(pool.pool, {set});
      pool.allocatedSets--;
      return;
    } catch (...) {
      continue;  // 该池不包含此描述符集
    }
  }
  throw std::runtime_error("Descriptor set not found in any pool");
}

void VKDescriptorPool::Reset() {
  for (auto& pool : m_pools) {
    m_device.resetDescriptorPool(pool.pool);
    pool.allocatedSets = 0;
  }
}