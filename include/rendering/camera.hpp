#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief 相机移动方向枚举
 */
enum class CameraMovement { Forward, Backward, Left, Right, Up, Down };

/**
 * @brief 简化的相机类
 *
 * 提供基本的3D相机功能：位置移动和缩放。
 * 使用lookAt方式，适合查看场景物体。
 */
class Camera {
 public:
  /**
   * @brief 相构造参数
   */
  struct CreateInfo {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    float zoomSensitivity = 1.0f;
    float minFov = 1.0f;
    float maxFov = 89.0f;
  }; /**
      * @brief 构造函数
      */
  explicit Camera(const CreateInfo& createInfo = {});

  /**
   * @brief 析构函数
   */
  ~Camera() = default;

  /**
   * @brief 处理键盘移动输入
   * @param movement 移动方向
   * @param deltaTime 帧时间间隔
   */
  void ProcessMovement(CameraMovement movement, float deltaTime);

  /**
   * @brief 处理鼠标移动（拖动控制视角）
   * @param xOffset X轴偏移
   * @param yOffset Y轴偏移
   */
  void ProcessMouseMovement(float xOffset, float yOffset);

  /**
   * @brief 处理鼠标滚轮缩放
   * @param yOffset 滚轮偏移
   */
  void ProcessMouseScroll(float yOffset);

  /**
   * @brief 设置相机位置
   * @param position 新位置
   */
  void SetPosition(const glm::vec3& position);

  /**
   * @brief 设置相机目标点
   * @param target 目标点
   */
  void SetTarget(const glm::vec3& target);

  /**
   * @brief 设置投影参数
   * @param fov 视野角度
   * @param aspectRatio 宽高比
   * @param nearPlane 近平面
   * @param farPlane 远平面
   */
  void SetProjection(float fov, float aspectRatio, float nearPlane,
                     float farPlane);

  /**
   * @brief 重置相机到初始状态
   */
  void Reset();

  // Getter方法
  const glm::vec3& GetPosition() const { return m_position; }
  const glm::vec3& GetTarget() const { return m_target; }
  const glm::vec3& GetUp() const { return m_up; }
  float GetFov() const { return m_fov; }
  float GetAspectRatio() const { return m_aspectRatio; }
  float GetNearPlane() const { return m_nearPlane; }
  float GetFarPlane() const { return m_farPlane; }
  float GetMovementSpeed() const { return m_movementSpeed; }
  float GetMouseSensitivity() const { return m_mouseSensitivity; }
  float GetZoomSensitivity() const { return m_zoomSensitivity; }

  /**
   * @brief 获取视图矩阵
   * @return 视图矩阵
   */
  glm::mat4 GetViewMatrix() const;

  /**
   * @brief 获取投影矩阵
   * @return 投影矩阵
   */
  glm::mat4 GetProjectionMatrix() const;

  /**
   * @brief 获取视图-投影矩阵
   * @return VP矩阵
   */
  glm::mat4 GetViewProjectionMatrix() const;
  // Setter方法
  void SetMovementSpeed(float speed) { m_movementSpeed = speed; }
  void SetMouseSensitivity(float sensitivity) {
    m_mouseSensitivity = sensitivity;
  }
  void SetZoomSensitivity(float sensitivity) {
    m_zoomSensitivity = sensitivity;
  }

 private:
  // 相机参数
  glm::vec3 m_position;
  glm::vec3 m_target;
  glm::vec3 m_up;

  // 投影参数
  float m_fov;
  float m_aspectRatio;
  float m_nearPlane;
  float m_farPlane;
  // 控制参数
  float m_movementSpeed;
  float m_mouseSensitivity;
  float m_zoomSensitivity;

  // 约束参数
  float m_minFov;
  float m_maxFov;

  // 初始参数（用于重置）
  CreateInfo m_initialParams;

  // 缓存的矩阵（避免重复计算）
  mutable glm::mat4 m_viewMatrix = glm::mat4(1.0f);
  mutable glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
  mutable bool m_viewMatrixDirty = true;
  mutable bool m_projectionMatrixDirty = true;

  /**
   * @brief 计算相机的前、右、上向量
   */
  glm::vec3 GetForwardVector() const;
  glm::vec3 GetRightVector() const;

  /**
   * @brief 更新视图矩阵
   */
  void UpdateViewMatrix() const;

  /**
   * @brief 更新投影矩阵
   */
  void UpdateProjectionMatrix() const;
};