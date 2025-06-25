#pragma once
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../core/Log.hpp"
#include "../core/interface/API.hpp"
#include "../rendering/camera.hpp"

class Window {
 public:
  Window(int width, int height, API api, const std::string& title)
      : m_width(width),
        m_height(height),
        m_title(title),
        m_api(api),
        m_window(nullptr),
        m_camera(nullptr),
        m_lastX(width / 2.0f),
        m_lastY(height / 2.0f),
        m_firstMouse(true),
        m_mousePressed(false),
        m_deltaTime(0.0f),
        m_lastFrame(0.0f) {}

  ~Window() { glfwTerminate(); }
  void createContext();
  void createOpenGLContext();
  void createVulkanContext();

  // 创建Vulkan表面
  vk::SurfaceKHR createVulkanSurface(vk::Instance instance);

  // Camera管理
  void SetCamera(Camera* camera) { m_camera = camera; }
  Camera* GetCamera() const { return m_camera; }

  // 输入处理
  void ProcessInput();
  void UpdateDeltaTime();
  bool ShouldClose() const { return glfwWindowShouldClose(m_window); }
  void SwapBuffers() { glfwSwapBuffers(m_window); }
  void PollEvents() const { glfwPollEvents(); }

  // 窗口操作
  void showWindow() {
    if (m_window) glfwShowWindow(m_window);
  }
  void hideWindow() {
    if (m_window) glfwHideWindow(m_window);
  }
  void focusWindow() {
    if (m_window) glfwFocusWindow(m_window);
  }

  // 获取窗口属性
  GLFWwindow* getHandle() const { return m_window; }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  float GetAspectRatio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
  }
  // 静态回调函数
  static void ScrollCallback(GLFWwindow* window, double xoffset,
                             double yoffset);
  static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);
  static void FramebufferSizeCallback(GLFWwindow* window, int width,
                                      int height);

 private:
  int m_width;
  int m_height;
  std::string m_title;
  API m_api;
  GLFWwindow* m_window;
  // Camera相关
  Camera* m_camera;
  float m_lastX;
  float m_lastY;
  bool m_firstMouse;
  bool m_mousePressed;  // 鼠标按钮状态

  // 时间相关
  float m_deltaTime;
  float m_lastFrame;
};
