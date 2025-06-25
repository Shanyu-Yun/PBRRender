#pragma once
#include "Vertex.hpp"

struct Model {
  std::string name;
  bool isValid = false;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};