#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <vulkan/vulkan.h>
#include <zlib.h>

void writeOpenGLVersion(std::ofstream& file) {
    if (!glfwInit()) {
        file << "Failed to initialize GLFW" << std::endl;
        return;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Version Checker", NULL, NULL);
    if (!window) {
        file << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    const GLubyte* version = glGetString(GL_VERSION);
    file << "OpenGL version: " << version << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();
}



void writeVulkanVersion(std::ofstream& file) {
    uint32_t apiVersion = 0;
    vkEnumerateInstanceVersion(&apiVersion);

    uint32_t major = VK_VERSION_MAJOR(apiVersion);
    uint32_t minor = VK_VERSION_MINOR(apiVersion);
    uint32_t patch = VK_VERSION_PATCH(apiVersion);

    file << "Vulkan API version: " << major << "." << minor << "." << patch << std::endl;
}

void writeGLFWVersion(std::ofstream& file) {
    glfwInit();
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    file << "GLFW version: " << major << "." << minor << "." << rev << std::endl;
    glfwTerminate();
}

void writeZlibVersion(std::ofstream& file) {
    file << "Zlib version: " << ZLIB_VERSION << std::endl;
}

int main() {
    std::ofstream file("library_versions.txt");

    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return -1;
    }

    writeOpenGLVersion(file);
    writeVulkanVersion(file);
    writeGLFWVersion(file);
    writeZlibVersion(file);

    file.close();
    return 0;
}
