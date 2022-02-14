#version 330 core

// 2D 平面顶点着色器器

layout (location = 0) in vec3 verPos;// 顶点坐标
layout (location = 2) in vec2 texPos;// 纹理坐标

uniform mat4 modelMat;

out vec2 outTexPos;

void main() {
    gl_Position = modelMat * vec4(verPos, 1.0); // NDC -1~1
    outTexPos = texPos;
}
