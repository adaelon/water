#include "GerstnerWave.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

static inline float rand(float min, float max, int precision = 1000)
{
    if (min > max) std::swap(min, max);
    float delta = max - min;
    auto i = int(delta * precision);
    return ((float)(rand() % i) / (float)precision) + min;
}

void setGersterWaveData(unsigned int shaderProgram, glm::vec2 windDir, int waveCount, GerstnerWave *waves)
{
    srand((unsigned int)time(nullptr));
    int r = rand() % 100;
    windDir = glm::vec2(cosf(r), sinf(r));
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "waveCount"), waveCount);
    for (int i = 0; i < waveCount; ++i) {
        waves[i].A = rand(0.01f, 0.05f);
        glUniform1f(glGetUniformLocation(shaderProgram, ("waves[" + std::to_string(i) + "].A").c_str()), waves[i].A);

        waves[i].Q = rand(0.3f, 0.4f);
        glUniform1f(glGetUniformLocation(shaderProgram, ("waves[" + std::to_string(i) + "].Q").c_str()), waves[i].Q);

        //风向
        float windAngle = acosf((windDir.x / sqrtf(windDir.x * windDir.x + windDir.y * windDir.y)));
        if (windDir.y < 0) windAngle = -windAngle;
        float waveAngle = rand(windAngle - glm::radians(60.0f), windAngle + glm::radians(60.0f));
        waves[i].D.x = cos(waveAngle);
        waves[i].D.y = sin(waveAngle);
        glUniform2fv(glGetUniformLocation(shaderProgram, ("waves[" + std::to_string(i) + "].D").c_str()), 1, glm::value_ptr(waves[i].D));

        waves[i].s = rand(0.5f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, ("waves[" + std::to_string(i) + "].s").c_str()), waves[i].s);

        waves[i].l = waves[i].A * rand(30.0f, 60.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, ("waves[" + std::to_string(i) + "].l").c_str()), waves[i].l);
    }
}
