#version 450
#extension GL_ARB_separate_shader_objects : enable

// 输入
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

// 输出
layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) out mat3 outTBN;

// 统一变量
layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
}
ubo;

void main() {
  // 计算世界坐标位置
  vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
  outWorldPos = worldPos.xyz;

  // 法线变换到世界空间
  outNormal = mat3(transpose(inverse(ubo.model))) * inNormal;

  // 传递纹理坐标
  outTexCoord = inTexCoord;

  // 计算切线空间到世界空间的变换矩阵
  vec3 N = normalize(outNormal);
  vec3 T = normalize(mat3(ubo.model) * inTangent);
  T = normalize(T - dot(T, N) * N);  // 施密特正交化
  vec3 B = cross(N, T);
  outTBN = mat3(T, B, N);

  // 输出裁剪空间坐标
  gl_Position = ubo.proj * ubo.view * worldPos;
}
