

#ifndef PROJECT_GERSTNERWAVES_H
#define PROJECT_GERSTNERWAVES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



struct GerstnerWave
{
    float Q;
    float A;
    glm::vec2 D;
    float l;
    float s;
};


void setGersterWaveData(unsigned int shaderProgram, glm::vec2 windDir, int waveCount, GerstnerWave *waves);


#endif //PROJECT_GERSTNERWAVES_H
