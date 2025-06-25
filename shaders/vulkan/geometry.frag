#version 450
#extension GL_ARB_separate_shader_objects : enable

// 输入
layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in mat3 inTBN;

// G-Buffer输出
layout(location = 0) out vec4 outPosition;  // 世界空间位置
layout(location = 1) out vec4 outNormal;    // 世界空间法线
layout(location = 2) out vec4 outAlbedo;    // 反照率(基础颜色)
layout(location = 3) out vec4 outMaterialProps;  // 金属度(R)、粗糙度(G)和AO(B)

// 统一变量 - MVP矩阵
layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
}
ubo;

// PBR贴图采样 - 与GeometryStage的描述符绑定匹配
layout(binding = 1) uniform sampler2D albedoMap;     // 反照率贴图
layout(binding = 2) uniform sampler2D normalMap;     // 法线贴图
layout(binding = 3) uniform sampler2D metallicMap;   // 金属度贴图
layout(binding = 4) uniform sampler2D roughnessMap;  // 粗糙度贴图
layout(binding = 5) uniform sampler2D aoMap;         // 环境光遮蔽贴图
layout(binding = 6) uniform sampler2D emissiveMap;   // 自发光贴图

void main() {
  // 写入世界空间位置
  outPosition = vec4(inWorldPos, 1.0);

  // 法线贴图处理 - 从切线空间转换到世界空间
  vec3 normal = texture(normalMap, inTexCoord).rgb;
  normal = normalize(normal * 2.0 - 1.0);  // 从[0,1]转换到[-1,1]
  normal = normalize(inTBN * normal);
  outNormal = vec4(normal, 1.0);

  // 反照率 + 自发光（alpha通道用于自发光强度）
  vec3 albedo = texture(albedoMap, inTexCoord).rgb;
  float emissiveIntensity = texture(emissiveMap, inTexCoord).r;
  outAlbedo = vec4(albedo, emissiveIntensity);

  // PBR材质参数
  float metallic = 1.0;
  float roughness = texture(roughnessMap, inTexCoord).r;
  float ao = texture(aoMap, inTexCoord).r;

  // 打包PBR材质参数
  outMaterialProps = vec4(metallic, roughness, ao, 1.0);
}
