#include <filesystem>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "core/Log.hpp"
#include "core/interface/API.hpp"
#include "platform/WindowHandler.hpp"
#include "platform/vulkan/VKContext.hpp"
#include "platform/vulkan/VKRender.hpp"
#include "rendering/camera.hpp"
#include "resource/AssetManager.hpp"

// 创建正方形模型的函数
Model CreateSquareModel(float size = 1.0f) {
  Model model;
  model.name = "Square";
  model.isValid = true;

  float halfSize = size * 0.5f;

  // 正方形的四个顶点（在XY平面上，Z=0）
  // 按逆时针顺序定义，正面朝向Z+方向
  model.vertices = {
      // 左下角
      {{-halfSize, -halfSize, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      // 右下角
      {{halfSize, -halfSize, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      // 右上角
      {{halfSize, halfSize, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      // 左上角
      {{-halfSize, halfSize, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}};

  // 索引数组，定义两个三角形组成正方形
  model.indices = {
      0, 1, 2,  // 第一个三角形
      2, 3, 0   // 第二个三角形
  };

  return model;
}

int main() {
  API enableApi = API::Vulkan;
  auto squareModel = CreateSquareModel();

  AssetManager assetManager(enableApi, false);
  assetManager.loadTexture(
      "E:\\GITHUB\\PBRRender\\resource\\material\\metal\\Metal055A_4K-JPG_"
      "Color.jpg",
      TextureType::Albedo, TextureFilter::Linear, "color");

  Material material;
  MaterialInput<glm::vec4> baseColorInput;
  baseColorInput.texture = assetManager.getTexture("color");
  material.name = "Metal";
  material.baseColor = baseColorInput;

  Window windowsHandler(800, 600, enableApi, "PBR Renderer");
  windowsHandler.createContext();
  VKRender vkRender;
  vkRender.init(&windowsHandler);
  Log::Shutdown();
}
