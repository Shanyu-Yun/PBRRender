#pragma once
#include "../../platform/windowhandler.hpp"
#include "../../resource/material.hpp"
#include "../../resource/model.hpp"

class IRenderer {
 public:
  virtual ~IRenderer() = default;

  virtual bool init(Window* windowHandle) = 0;
  virtual void resize(int width, int height) = 0;
  virtual void renderFrame() = 0;
  virtual void setModel(const Model& model) = 0;
  virtual void setMaterial(const Material& material) = 0;
  virtual void setCamera() = 0;

 protected:
  Window* m_windowHandle = nullptr;
};
