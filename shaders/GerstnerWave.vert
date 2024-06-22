#version 430 core

struct Wave {
    float Q;
    float A;
    vec2 D;
    float l;
    float s;
};

layout (location = 0) in vec3 initPos;

out VS_OUT {
    vec4 fragPos;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform int waveCount;
uniform Wave waves[10];

void main()
{
    vec3 pos = initPos;

    float u = pos.x, v = pos.z;

    for (int i = 0; i < waveCount; ++i) {

        float w = 2 * 3.14 / waves[i].l;
        float fi = 2 * 3.14 * waves[i].s / waves[i].l;
        pos.x += waves[i].Q * waves[i].A * waves[i].D.x
                 * cos(dot(waves[i].D, vec2(u, v)) * w + fi * time);
        pos.y += waves[i].A * sin(dot(waves[i].D, vec2(u, v)) * w + fi * time);
        pos.z += waves[i].Q * waves[i].A * waves[i].D.y
                 * cos(dot(waves[i].D, vec2(u, v)) * w + fi * time);
    }
    vs_out.fragPos = model * vec4(pos, 1.0);


    vs_out.normal = vec3(0.0, 1.0, 0.0);
    for (int i = 0; i < waveCount; ++i) {
        float w = 2 * 3.14 / waves[i].l;
        float fi = 2 * 3.14 * waves[i].s / waves[i].l;
        vs_out.normal.x += (-waves[i].D.x) * w * waves[i].A
                           * cos(dot(waves[i].D, vec2(u, v)) * w + fi * time);
        vs_out.normal.y -= waves[i].Q * w * waves[i].A * sin(dot(waves[i].D, vec2(u, v)) * w + fi * time);
        vs_out.normal.z += (-waves[i].D.y) * w * waves[i].A
                           * cos(dot(waves[i].D, vec2(u, v)) * w + fi * time);
    }

    vs_out.normal = mat3(transpose(inverse(model))) * vs_out.normal;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}
