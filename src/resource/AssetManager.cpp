#include "resource/AssetManager.hpp"

#include <iostream>

#include "core/Log.hpp"
#include "third_party/stb_image.h"
#include "third_party/tiny_obj_loader.h"

void AssetManager::loadTexture(std::string path, TextureType type,
                               TextureFilter filter, std::string name) {
  int w, h, c;
  if (api_ == API::OpenGL)
    stbi_set_flip_vertically_on_load(true);
  else if (api_ == API::Vulkan)
    stbi_set_flip_vertically_on_load(false);

  // 强制加载为RGBA格式以确保Vulkan兼容性
  uint8_t* data = stbi_load(path.c_str(), &w, &h, &c, STBI_rgb_alpha);
  if (!data) {
    Log::LogMessage(Log::Level::Error, "Failed to load texture: " + path);
    return;
  }

  // 现在数据总是RGBA格式（4通道）
  std::shared_ptr<uint8_t[]> buffer(new uint8_t[w * h * 4]);
  std::memcpy(buffer.get(), data, w * h * 4);
  stbi_image_free(data);

  // 创建纹理对象，指定通道类型为RGBA
  Texture texture(name, w, h, 4, type, filter, ChannelType::RGBA);
  texture.data = buffer;

  //这里还需要一些逻辑检查是否有重名纹理

  if (texture.IsValid()) {
    textures[name] = texture;
    Log::LogMessage(Log::Level::Info, "Texture loaded: " + name);
  } else {
    Log::LogMessage(Log::Level::Error, "Invalid texture: " + name);
  }
}

void AssetManager::loadModel(std::string path, std::string name) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  bool ret =
      tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
  if (!warn.empty()) {
    Log::LogMessage(Log::Level::Warning, "OBJ Load Warning: " + warn);
  }
  if (!err.empty()) {
    Log::LogMessage(Log::Level::Error, "OBJ Load Error: " + err);
  }
  if (!ret) {
    Log::LogMessage(Log::Level::Error, "Failed to load model: " + path);
    return;
  }
  Model model;
  model.name = name;
  model.isValid = true;
  std::unordered_map<Vertex, uint32_t> uniqueVertices;
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex;
      vertex.position = glm::vec3(attrib.vertices[3 * index.vertex_index + 0],
                                  attrib.vertices[3 * index.vertex_index + 1],
                                  attrib.vertices[3 * index.vertex_index + 2]);
      vertex.normal = glm::vec3(attrib.normals[3 * index.normal_index + 0],
                                attrib.normals[3 * index.normal_index + 1],
                                attrib.normals[3 * index.normal_index + 2]);
      if (api_ == API::OpenGL) {
        vertex.texCoord =
            glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],
                      1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
      } else {
        vertex.texCoord =
            glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],
                      attrib.texcoords[2 * index.texcoord_index + 1]);
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
        model.vertices.push_back(vertex);
      }
      model.indices.push_back(uniqueVertices[vertex]);
    }
  }
  models[name] = model;
  Log::LogMessage(Log::Level::Info, "Model loaded: " + name);
}

void AssetManager::loadMaterial(std::string path, std::string name) {}

Material AssetManager::getMaterial(const std::string& name) {
  auto it = materials.find(name);
  if (it != materials.end()) {
    return it->second;
  };
  return Material();
}

Texture AssetManager::getTexture(const std::string& name) {
  auto it = textures.find(name);
  if (it != textures.end()) {
    return it->second;
  }
  return Texture();
}

Model AssetManager::getModel(const std::string& name) {
  auto it = models.find(name);
  if (it != models.end()) {
    return it->second;
  }
  return Model();
}
