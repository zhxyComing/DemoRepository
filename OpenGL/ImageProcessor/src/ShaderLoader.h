//
// Created by 徐政 on 2021/11/11.
// 加载着色器程序
//

#ifndef GLREVIEW_SHADERLOADER_H
#define GLREVIEW_SHADERLOADER_H

#include "iostream"
#include "glad/glad.h"
#include <fstream>
#include <sstream>

namespace ShaderLoader {

    // 加载Shader 返回着色器程序
    static unsigned int loadShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
        using namespace std;
        // 加载着色器代码
        string vShaderCode, fShaderCode;
        ifstream vShaderFile, fShaderFile;
        vShaderFile.exceptions(ifstream::badbit);
        fShaderFile.exceptions(ifstream::badbit);
        try {
            vShaderFile.open(vertexShaderPath);
            fShaderFile.open(fragmentShaderPath);
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vShaderCode = vShaderStream.str();
            fShaderCode = fShaderStream.str();
        } catch (ifstream::failure &e) {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESS_READ" << endl;
            return 0;
        }
        const GLchar *vShaderCodeC = vShaderCode.c_str();
        const GLchar *fShaderCodeC = fShaderCode.c_str();
        // 创建_加载_编译_检测着色器
        GLint success;
        GLchar infoLog[512];

        GLuint vShader;
        vShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vShader, 1, &vShaderCodeC, nullptr);
        glCompileShader(vShader);
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
            return 0;
        }

        GLuint fShader;
        fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fShader, 1, &fShaderCodeC, nullptr);
        glCompileShader(fShader);
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vShader);
        glAttachShader(program, fShader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << endl;
            return 0;
        }
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return program;
    }
}

#endif //GLREVIEW_SHADERLOADER_H
