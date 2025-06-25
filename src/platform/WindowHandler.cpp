#define GLFW_INCLUDE_VULKAN
#include "platform/WindowHandler.hpp"

void Window::createContext() {
  glfwInit();
  if (m_api == API::OpenGL) {
    createOpenGLContext();
  } else if (m_api == API::Vulkan) {
    createVulkanContext();
  } else {
    Log::LogMessage(Log::Level::Error, "Unsupported API type");
  }
}

void Window::createOpenGLContext() {
  // 配置OpenGL上下文
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  m_window = glfwCreateWindow(
      m_width, m_height, (m_title + " [OpenGL]").c_str(), nullptr, nullptr);
  if (!m_window) {
    Log::LogMessage(Log::Level::Error, "Failed to create OpenGL window");
  }
  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1);  // 启用垂直同步

  // 设置窗口用户指针和回调
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetScrollCallback(m_window, ScrollCallback);
  glfwSetCursorPosCallback(m_window, MouseCallback);
  glfwSetMouseButtonCallback(m_window, MouseButtonCallback);

  Log::LogMessage(Log::Level::Info, "OpenGL context created successfully");

  // 窗口创建后显示
  glfwShowWindow(m_window);
}

void Window::createVulkanContext() {
  // 配置Vulkan窗口
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  m_window = glfwCreateWindow(
      m_width, m_height, (m_title + " [Vulkan]").c_str(), nullptr, nullptr);
  if (!m_window) {
    Log::LogMessage(Log::Level::Error, "Failed to create Vulkan window");
    return;
  }
  // 设置窗口用户指针和回调
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetScrollCallback(m_window, ScrollCallback);
  glfwSetCursorPosCallback(m_window, MouseCallback);
  glfwSetMouseButtonCallback(m_window, MouseButtonCallback);

  // 设置回调
  Log::LogMessage(Log::Level::Info, "Vulkan window created successfully");

  // 窗口创建后显示
  glfwShowWindow(m_window);
}

vk::SurfaceKHR Window::createVulkanSurface(vk::Instance instance) {
  if (m_api != API::Vulkan) {
    Log::LogMessage(Log::Level::Error,
                    "Cannot create Vulkan surface without Vulkan context");
  }

  VkSurfaceKHR vkSurface;
  if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), m_window,
                              nullptr, &vkSurface) != VK_SUCCESS) {
    Log::LogMessage(Log::Level::Error,
                    "Failed to create Vulkan window surface");
  }
  vk::SurfaceKHR surface(vkSurface);

  return surface;
}

void Window::ProcessInput() {
  if (!m_window || !m_camera) return;

  // ESC键退出
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_window, true);
  }

  // 相机移动控制
  if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Forward, m_deltaTime);
  }
  if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Backward, m_deltaTime);
  }
  if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Left, m_deltaTime);
  }
  if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Right, m_deltaTime);
  }
  if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Up, m_deltaTime);
  }
  if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
    m_camera->ProcessMovement(CameraMovement::Down, m_deltaTime);
  }
}

void Window::UpdateDeltaTime() {
  float currentFrame = static_cast<float>(glfwGetTime());
  m_deltaTime = currentFrame - m_lastFrame;
  m_lastFrame = currentFrame;
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset,
                            double yoffset) {
  // 从窗口用户指针获取Window实例
  Window* windowInstance =
      static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (windowInstance && windowInstance->m_camera) {
    windowInstance->m_camera->ProcessMouseScroll(static_cast<float>(yoffset));
  }
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width,
                                     int height) {
  // 从窗口用户指针获取Window实例
  Window* windowInstance =
      static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (windowInstance) {
    windowInstance->m_width = width;
    windowInstance->m_height = height;

    // 更新相机的宽高比
    if (windowInstance->m_camera) {
      float aspectRatio =
          static_cast<float>(width) / static_cast<float>(height);
      windowInstance->m_camera->SetProjection(
          windowInstance->m_camera->GetFov(), aspectRatio,
          windowInstance->m_camera->GetNearPlane(),
          windowInstance->m_camera->GetFarPlane());
    }

    Log::LogMessage(Log::Level::Info,
                    "Framebuffer size changed to: " + std::to_string(width) +
                        "x" + std::to_string(height));
  }
}

void Window::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
  Window* windowInstance =
      static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (!windowInstance) return;

  float xposf = static_cast<float>(xpos);
  float yposf = static_cast<float>(ypos);

  if (windowInstance->m_firstMouse) {
    windowInstance->m_lastX = xposf;
    windowInstance->m_lastY = yposf;
    windowInstance->m_firstMouse = false;
  }

  float xoffset = xposf - windowInstance->m_lastX;
  float yoffset = windowInstance->m_lastY - yposf;  // 反转Y坐标

  windowInstance->m_lastX = xposf;
  windowInstance->m_lastY = yposf;

  // 只有在鼠标按下时才处理移动
  if (windowInstance->m_mousePressed && windowInstance->m_camera) {
    windowInstance->m_camera->ProcessMouseMovement(xoffset, yoffset);
  }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action,
                                 int mods) {
  Window* windowInstance =
      static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (!windowInstance) return;

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      windowInstance->m_mousePressed = true;
      // 获取当前鼠标位置，避免跳跃
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      windowInstance->m_lastX = static_cast<float>(xpos);
      windowInstance->m_lastY = static_cast<float>(ypos);
    } else if (action == GLFW_RELEASE) {
      windowInstance->m_mousePressed = false;
    }
  }
}