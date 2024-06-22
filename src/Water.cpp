#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "GerstnerWave.h"

GLFWwindow *init();
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
// 生成顶点数据
void genWaterVertexBuffer(int width, float *vertices, unsigned int *indices);

// 摄像机变量
glm::vec3 cameraPos   = glm::vec3(0.0f, 10.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw   = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
float fov = 45.0f;

int gScreenWidth = 800;
int gScreenHeight = 600;
float gDeltaTime = 0.0f;
float gLastFrame = 0.0f;

// FPS变量
float lastTime = 0.0f;
int nbFrames = 0;

unsigned int loadShaders(const char* vertexPath, const char* fragmentPath);

int main()
{
    GLFWwindow *window = init();


    // 加载着色器
    unsigned int shaderProgram = loadShaders("E:/allwork/water/shaders/GerstnerWave.vert", "E:/allwork/water/shaders/Water.frag");


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 初始化水面顶点数据
    int width = 200;
    int vertexCount = 3 * width * width;
    auto *vertices = new float[vertexCount];
    int indexCount = 2 * 3 * width * width;
    auto *indices = new unsigned int[indexCount];
    genWaterVertexBuffer(width, vertices, indices);

    unsigned int VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec2 windDir = glm::vec2(0.7f, 0.5f);
    int waveCount = 6;
    GerstnerWave waves[6];

    setGersterWaveData(shaderProgram, windDir, waveCount, waves);


    while (!glfwWindowShouldClose(window)) {
        // 计算每帧时间差
        auto currentFrame = (float)glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // 处理用户输入
        processInput(window);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)gScreenWidth / gScreenHeight, 0.1f, 100.0f);

        glUseProgram(shaderProgram);
        // 设置顶点着色器数据
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int timeLoc = glGetUniformLocation(shaderProgram, "time");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniform1f(timeLoc, (float)glfwGetTime());

        // 设置片段着色器数据
        int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        int waterColorLoc = glGetUniformLocation(shaderProgram, "waterColor");
        int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        glUniform3fv(waterColorLoc, 1, glm::value_ptr(glm::vec3(0.1f, 0.3f, 0.5f)));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::vec3(-1.0f, -1.0f, 2.0f)));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

        nbFrames++;
        if (currentFrame - lastTime >= 1.0) {
            std::stringstream ss;
            ss << "FPS: " << nbFrames;
            std::cout << ss.str() << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] vertices;
    delete[] indices;
    glfwTerminate();
    return 0;
}

unsigned int loadShaders(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout <<"ERROR"<< std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);

    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);

    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Failed" << std::endl;

    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram;
}

void genWaterVertexBuffer(int width, float *vertices, unsigned int *indices)
{
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < width; ++j) {
            vertices[3 * (i * width + j)] = (i - width / 2.0f) / 10.0f;
            vertices[3 * (i * width + j) + 1] = 0;
            vertices[3 * (i * width + j) + 2] = (j - width / 2.0f) / 10.0f;
        }
    }
    for (unsigned int i = 0; i < width - 1; ++i) {
        for (unsigned int j = 0; j < width - 1; ++j) {
            indices[6 * (i * width + j)] = (i * width + j);
            indices[6 * (i * width + j) + 1] = (i * width + j + 1);
            indices[6 * (i * width + j) + 2] = ((i + 1) * width + j);
            indices[6 * (i * width + j) + 3] = (i * width + j + 1);
            indices[6 * (i * width + j) + 4] = ((i + 1) * width + j);
            indices[6 * (i * width + j) + 5] = ((i + 1) * width + j + 1);
        }
    }
}

GLFWwindow *init()
{
    // 初始化 GLFW 上下文
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // 创建窗口对象
    GLFWwindow *window = glfwCreateWindow(gScreenWidth, gScreenHeight, "窗口标题", nullptr, nullptr);


    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "初始化 GLAD 失败" << std::endl;
        return nullptr;
    }


    glViewport(0, 0, gScreenWidth, gScreenHeight);


    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // 设置鼠标输入
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    return window;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    gScreenWidth = width;
    gScreenHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 2.5f * gDeltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
