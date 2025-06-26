cmake_minimum_required(VERSION 3.10)

project(VkRenderer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 指定 vcpkg 目录并让 CMake 自动找到库
set(CMAKE_TOOLCHAIN_FILE "E:/enviroment/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# 查找 Vulkan 库
find_package(Vulkan REQUIRED)

# 查找 GLFW 库
find_package(glfw3 REQUIRED)

# 查找 GLM 库
find_package(glm REQUIRED)


# 包含头文件目录
include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

# 添加源文件
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_definitions(-DUSE_VULKAN)
# 生成可执行文件
add_executable(real_renderer ${SOURCES})

# 链接 Vulkan 和 GLFW 库
target_link_libraries(real_renderer Vulkan::Vulkan glfw glm::glm)

# 包含头文件目录
target_include_directories(real_renderer PRIVATE include)