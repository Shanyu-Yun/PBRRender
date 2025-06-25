#include "resource/ModelGenerator.hpp"

#include <cmath>


Model ModelGenerator::CreateSquare(float size, const std::string& name) {
  Model model;
  model.name = name;
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

Model ModelGenerator::CreateCube(float size, const std::string& name) {
  Model model;
  model.name = name;
  model.isValid = true;

  float halfSize = size * 0.5f;

  // 立方体的24个顶点（每个面4个顶点，共6个面）
  model.vertices = {
      // 前面 (Z+)
      {{-halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      {{halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

      // 后面 (Z-)
      {{halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
      {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
      {{-halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
      {{halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

      // 左面 (X-)
      {{-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-halfSize, -halfSize, halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
      {{-halfSize, halfSize, halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{-halfSize, halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

      // 右面 (X+)
      {{halfSize, -halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
      {{halfSize, halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{halfSize, halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

      // 上面 (Y+)
      {{-halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
      {{halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
      {{-halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

      // 下面 (Y-)
      {{-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
      {{halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
      {{halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
      {{-halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}};

  // 立方体的36个索引（每个面2个三角形，共12个三角形）
  model.indices = {// 前面
                   0, 1, 2, 2, 3, 0,
                   // 后面
                   4, 5, 6, 6, 7, 4,
                   // 左面
                   8, 9, 10, 10, 11, 8,
                   // 右面
                   12, 13, 14, 14, 15, 12,
                   // 上面
                   16, 17, 18, 18, 19, 16,
                   // 下面
                   20, 21, 22, 22, 23, 20};

  return model;
}

Model ModelGenerator::CreatePlane(float width, float height, int widthSegments,
                                  int heightSegments, const std::string& name) {
  Model model;
  model.name = name;
  model.isValid = true;

  float halfWidth = width * 0.5f;
  float halfHeight = height * 0.5f;

  // 生成顶点
  for (int y = 0; y <= heightSegments; ++y) {
    for (int x = 0; x <= widthSegments; ++x) {
      float xPos = (static_cast<float>(x) / widthSegments - 0.5f) * width;
      float yPos = (static_cast<float>(y) / heightSegments - 0.5f) * height;
      float u = static_cast<float>(x) / widthSegments;
      float v = static_cast<float>(y) / heightSegments;

      Vertex vertex;
      vertex.position = {xPos, yPos, 0.0f};
      vertex.normal = {0.0f, 0.0f, 1.0f};
      vertex.texCoord = {u, v};

      model.vertices.push_back(vertex);
    }
  }

  // 生成索引
  for (int y = 0; y < heightSegments; ++y) {
    for (int x = 0; x < widthSegments; ++x) {
      int topLeft = y * (widthSegments + 1) + x;
      int topRight = topLeft + 1;
      int bottomLeft = (y + 1) * (widthSegments + 1) + x;
      int bottomRight = bottomLeft + 1;

      // 第一个三角形
      model.indices.push_back(topLeft);
      model.indices.push_back(bottomLeft);
      model.indices.push_back(topRight);

      // 第二个三角形
      model.indices.push_back(topRight);
      model.indices.push_back(bottomLeft);
      model.indices.push_back(bottomRight);
    }
  }

  return model;
}

glm::vec3 ModelGenerator::CalculateNormal(const glm::vec3& v1,
                                          const glm::vec3& v2,
                                          const glm::vec3& v3) {
  glm::vec3 edge1 = v2 - v1;
  glm::vec3 edge2 = v3 - v1;
  return glm::normalize(glm::cross(edge1, edge2));
}
