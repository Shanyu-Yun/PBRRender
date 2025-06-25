#pragma once
#include <glm/glm.hpp>
#include <string>

#include "Texture.hpp"

template <typename T>
struct MaterialInput {
  bool UseFallback = false;
  T value;
  Texture texture;
};

struct Material {
  std::string name;
  int materialNums = 0;  // 材质数量

  // Base color
  MaterialInput<glm::vec4> baseColor;  // 基础颜色

  // normal
  MaterialInput<float> normal;  // 法线贴图

  // metal
  MaterialInput<float> metallic;   // 金属度
  MaterialInput<float> roughness;  // 粗糙度

  // Ambient Occlusion
  MaterialInput<float> ao;  // 环境光遮蔽

  // Emissive
  MaterialInput<float> emissiveIntensity;  // 自发光强度

  // Height
  MaterialInput<float> heightScale;  // 高度图
};
