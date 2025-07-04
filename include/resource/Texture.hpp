#pragma once
#include <memory>
#include <string>

/**
 * @brief Texture type enumeration
 */
enum class TextureType {
  Albedo,            // BaseColor
  Normal,            // Normal map
  Specular,          // Specular reflection map
  Roughness,         // Roughness map
  Metallic,          // Metallic map
  AmbientOcclusion,  // Ambient occlusion map
  Emissive,          // Emissive map
  HeightMap,         // Height map
  CubeMap,           // Cube map
  HDR,               // HDR map
  None,              // No texture
};

/**
 * @brief Texture filtering mode
 */
enum class TextureFilter {
  Nearest,  // Nearest neighbor filtering
  Linear,   // Linear filtering
  None,
};

/**
 * @brief Image channel type enumeration
 */
enum class ChannelType {
  R = 1,     // Single channel red (grayscale)
  RG = 2,    // Dual channel (red-green)
  RGB = 3,   // Three channel RGB
  RGBA = 4,  // Four channel RGBA
  BGR = 5,   // Three channel BGR
  BGRA = 6,  // Four channel BGRA
  None = 0,  // Invalid channel
};

struct Texture {
  std::string name;  // Texture name
  int width = -1;
  int height = -1;
  TextureType type = TextureType::None;
  TextureFilter filter = TextureFilter::None;
  ChannelType channelType = ChannelType::None;

  std::shared_ptr<uint8_t[]> data = nullptr;

  Texture() = default;
  Texture(const std::string& textureName, int w, int h,
          ChannelType ct = ChannelType::None, TextureType t = TextureType::None,
          TextureFilter f = TextureFilter::None)
      : name(textureName),
        width(w),
        height(h),
        channelType(ct),
        type(t),
        filter(f) {}

  bool IsValid() const { return width > 0 && height > 0; }

  // Get bytes per pixel
  int GetBytesPerPixel() const { return static_cast<int>(channelType); }

  // Get total bytes
  size_t GetTotalBytes() const {
    return static_cast<size_t>(width) * height * GetBytesPerPixel();
  }

  // Check if has alpha channel
  bool HasAlpha() const {
    return channelType == ChannelType::RGBA || channelType == ChannelType::BGRA;
  }
};
