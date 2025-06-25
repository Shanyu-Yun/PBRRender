#include "rendering/camera.hpp"

#include <algorithm>
#include <cmath>

Camera::Camera(const CreateInfo& createInfo)
    : m_position(createInfo.position),
      m_target(createInfo.target),
      m_up(createInfo.up),
      m_fov(createInfo.fov),
      m_aspectRatio(createInfo.aspectRatio),
      m_nearPlane(createInfo.nearPlane),
      m_farPlane(createInfo.farPlane),
      m_movementSpeed(createInfo.movementSpeed),
      m_mouseSensitivity(createInfo.mouseSensitivity),
      m_zoomSensitivity(createInfo.zoomSensitivity),
      m_minFov(createInfo.minFov),
      m_maxFov(createInfo.maxFov),
      m_initialParams(createInfo) {}

void Camera::ProcessMovement(CameraMovement movement, float deltaTime) {
  float velocity = m_movementSpeed * deltaTime;

  glm::vec3 forward = GetForwardVector();
  glm::vec3 right = GetRightVector();
  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

  switch (movement) {
    case CameraMovement::Forward:
      m_position += forward * velocity;
      break;
    case CameraMovement::Backward:
      m_position -= forward * velocity;
      break;
    case CameraMovement::Left:
      m_position -= right * velocity;
      break;
    case CameraMovement::Right:
      m_position += right * velocity;
      break;
    case CameraMovement::Up:
      m_position += worldUp * velocity;
      break;
    case CameraMovement::Down:
      m_position -= worldUp * velocity;
      break;
  }

  m_viewMatrixDirty = true;
}

void Camera::ProcessMouseScroll(float yOffset) {
  m_fov -= yOffset * m_zoomSensitivity;
  m_fov = std::clamp(m_fov, m_minFov, m_maxFov);
  m_projectionMatrixDirty = true;
}

void Camera::SetPosition(const glm::vec3& position) {
  m_position = position;
  m_viewMatrixDirty = true;
}

void Camera::SetTarget(const glm::vec3& target) {
  m_target = target;
  m_viewMatrixDirty = true;
}

void Camera::SetProjection(float fov, float aspectRatio, float nearPlane,
                           float farPlane) {
  m_fov = std::clamp(fov, m_minFov, m_maxFov);
  m_aspectRatio = aspectRatio;
  m_nearPlane = nearPlane;
  m_farPlane = farPlane;
  m_projectionMatrixDirty = true;
}

void Camera::Reset() {
  m_position = m_initialParams.position;
  m_target = m_initialParams.target;
  m_up = m_initialParams.up;
  m_fov = m_initialParams.fov;
  m_aspectRatio = m_initialParams.aspectRatio;
  m_nearPlane = m_initialParams.nearPlane;
  m_farPlane = m_initialParams.farPlane;

  m_viewMatrixDirty = true;
  m_projectionMatrixDirty = true;
}

glm::mat4 Camera::GetViewMatrix() const {
  if (m_viewMatrixDirty) {
    UpdateViewMatrix();
  }
  return m_viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
  if (m_projectionMatrixDirty) {
    UpdateProjectionMatrix();
  }
  return m_projectionMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
  return GetProjectionMatrix() * GetViewMatrix();
}

glm::vec3 Camera::GetForwardVector() const {
  return glm::normalize(m_target - m_position);
}

glm::vec3 Camera::GetRightVector() const {
  return glm::normalize(glm::cross(GetForwardVector(), m_up));
}

void Camera::UpdateViewMatrix() const {
  m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
  m_viewMatrixDirty = false;
}

void Camera::UpdateProjectionMatrix() const {
  m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio,
                                        m_nearPlane, m_farPlane);
  m_projectionMatrixDirty = false;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset) {
  xOffset *= m_mouseSensitivity;
  yOffset *= m_mouseSensitivity;

  // 计算相机到目标的距离
  float distance = glm::length(m_position - m_target);

  // 将偏移转换为球坐标系的角度变化
  float theta = xOffset * 0.01f;  // 水平旋转
  float phi = yOffset * 0.01f;    // 垂直旋转

  // 获取当前相机相对于目标的方向向量
  glm::vec3 direction = glm::normalize(m_position - m_target);

  // 转换为球坐标系
  float currentTheta = atan2(direction.z, direction.x);
  float currentPhi = asin(glm::clamp(direction.y, -1.0f, 1.0f));

  // 应用旋转
  currentTheta += theta;
  currentPhi += phi;

  // 约束垂直角度，避免翻转
  const float maxPhi = 1.5f;  // ~85度
  currentPhi = glm::clamp(currentPhi, -maxPhi, maxPhi);

  // 转换回笛卡尔坐标系
  glm::vec3 newDirection;
  newDirection.x = cos(currentPhi) * cos(currentTheta);
  newDirection.y = sin(currentPhi);
  newDirection.z = cos(currentPhi) * sin(currentTheta);

  // 更新相机位置，保持到目标的距离
  m_position = m_target + newDirection * distance;

  m_viewMatrixDirty = true;
}
