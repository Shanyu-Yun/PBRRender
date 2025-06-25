#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct Vertex {
  glm::vec3 position;  // 顶点位置
  glm::vec3 normal;    // 顶点法线
  glm::vec2 texCoord;  // 纹理坐标

  //
  static std::array<vk::VertexInputBindingDescription, 1>
  getBindingDescriptions() {
    return {vk::VertexInputBindingDescription{0, sizeof(Vertex),
                                              vk::VertexInputRate::eVertex}};
  }

  static std::array<vk::VertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    return std::array<vk::VertexInputAttributeDescription, 3>{
        vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat,
                                            offsetof(Vertex, position)},
        vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat,
                                            offsetof(Vertex, normal)},
        vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat,
                                            offsetof(Vertex, texCoord)}};
  }

  bool operator==(const Vertex& other) const {
    return position == other.position && normal == other.normal &&
           texCoord == other.texCoord;
  }
};

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(const Vertex& v) const {
    return ((hash<float>()(v.position.x) ^
             (hash<float>()(v.position.y) << 1)) >>
            1) ^
           (hash<float>()(v.position.z) << 1) ^
           (hash<float>()(v.normal.x) << 1) ^
           (hash<float>()(v.texCoord.x) << 1);
  }
};
};  // namespace std
