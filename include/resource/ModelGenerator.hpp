#pragma once
#include <glm/glm.hpp>

#include "Model.hpp"
#include "Vertex.hpp"


/**
 * @brief 模型生成器类
 *
 * 提供常见几何体的生成功能，如正方形、立方体、球体等
 */
class ModelGenerator {
 public:
  /**
   * @brief 生成正方形模型（在XY平面上）
   * @param size 正方形的边长
   * @param name 模型名称
   * @return 生成的正方形模型
   */
  static Model CreateSquare(float size = 1.0f,
                            const std::string& name = "Square");

  /**
   * @brief 生成立方体模型
   * @param size 立方体的边长
   * @param name 模型名称
   * @return 生成的立方体模型
   */
  static Model CreateCube(float size = 1.0f, const std::string& name = "Cube");

  /**
   * @brief 生成平面模型（可指定分辨率）
   * @param width 平面宽度
   * @param height 平面高度
   * @param widthSegments 宽度分段数
   * @param heightSegments 高度分段数
   * @param name 模型名称
   * @return 生成的平面模型
   */
  static Model CreatePlane(float width = 1.0f, float height = 1.0f,
                           int widthSegments = 1, int heightSegments = 1,
                           const std::string& name = "Plane");

 private:
  /**
   * @brief 计算平面法线
   * @param v1 第一个顶点
   * @param v2 第二个顶点
   * @param v3 第三个顶点
   * @return 计算得到的法线向量
   */
  static glm::vec3 CalculateNormal(const glm::vec3& v1, const glm::vec3& v2,
                                   const glm::vec3& v3);
};
