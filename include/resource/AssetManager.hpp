#pragma once
#include <string>
#include <unordered_map>

#include "..\core\interface\API.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Texture.hpp"

/**
 * @brief 资源管理器类
 *
 * 负责加载和管理纹理和模型资源。
 */

class AssetManager {
 public:
  AssetManager(API api, bool saveMaterial = false)
      : isSaveMaterial_(saveMaterial), api_(api) {}
  ~AssetManager() = default;

  void loadTexture(std::string path, TextureType type,
                   TextureFilter filter = TextureFilter::Linear,
                   std::string name = "");
  void loadModel(std::string path, std::string name = "");
  void loadMaterial(std::string path, std::string name = "");

  Material getMaterial(const std::string& name);
  Texture getTexture(const std::string& name);
  Model getModel(const std::string& name);

 private:
  bool isSaveMaterial_ = false;
  API api_ = API::OpenGL;
  std::unordered_map<std::string, Texture> textures;    // 纹理资源
  std::unordered_map<std::string, Model> models;        // 模型资源
  std::unordered_map<std::string, Material> materials;  // 材质资源

  void savematerial();
};