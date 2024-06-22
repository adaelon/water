cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)

# 设置项目名称
project(WaterProject)

# 查找必要的包
find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)

# 手动设置Vulkan的包含目录和库路径
set(VULKAN_INCLUDE_DIR "E:/allwork/Vulkan/Include")
set(VULKAN_LIBRARY "E:/allwork/Vulkan/Lib/vulkan-1.lib")

include_directories(${VULKAN_INCLUDE_DIR})
# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("E:/allwork/vcpkg/vcpkg/packages/bzip2_x64-windows/include")
include_directories("E:/allwork/vcpkg/vcpkg/packages/zlib_x64-windows/include")
include_directories("E:/allwork/vcpkg/vcpkg/packages/glm_x64-windows/include")

# 链接库目录
link_directories("E:/allwork/water/lib")

# 配置GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)



add_executable(Water
        src/Water.cpp
        src/GerstnerWave.cpp
        src/glad.c)


add_executable(OpenGLVersionChecker
        src/OpenGLVersionChecker.cpp
        src/glad.c)
# 链接库文件
target_link_libraries(Water glfw ${OPENGL_gl_LIBRARY} ${FREETYPE_LIBRARIES} ${VULKAN_LIBRARY}
        "E:/allwork/water/lib/bz2.lib"
        "E:/allwork/water/lib/zlib.lib")


target_link_libraries(OpenGLVersionChecker glfw ${OPENGL_gl_LIBRARY} ${FREETYPE_LIBRARIES} ${VULKAN_LIBRARY}
        "E:/allwork/water/lib/bz2.lib"
        "E:/allwork/water/lib/zlib.lib")
