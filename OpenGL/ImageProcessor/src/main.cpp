// 利用 OpenGL 处理图片，实现各种效果
// 只处理图片，不需要窗口、深度测试等。

#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "TextureHelper.h"
#include "ShaderLoader.h"
#include "gtc/type_ptr.hpp"
#include "stb_image_write.h"

static const char *importImagePath = "/Users/xuzheng/CLionProjects/ImageProcessor/res/image/awesomeface.png";
static const char *rectVertexShaderPath = "/Users/xuzheng/CLionProjects/ImageProcessor/src/glsl/SimpleTextureVertexShader.glsl";
static const char *rectFragmentShaderPath = "/Users/xuzheng/CLionProjects/ImageProcessor/src/glsl/SimpleTextureFragmentShader.glsl";

// 基础配置
void baseConfig();

// 图像处理
void imageRender();

// 图像导出
void imageExport();

void bindFrameBuffer();

// 导入图片
void importImage();

// 加载模型
void loadModel();

// 加载着色器
void loadShader();

void destroy();

void renderLoop();

// 导入的、要处理图像的纹理
unsigned int importTexture;

// FrameBuffer 绑定的纹理，也是最终效果的纹理
unsigned int fboTexture;

int view_port_width, view_port_height;

// 用于绘制顶点
unsigned int VAO, VBO, EBO;

// 着色器程序
unsigned int shaderProgram;

GLFWwindow *window = nullptr;

int main() {
    // ...
    // ❗️ 这里依赖 GLFW 窗口做 OpenGL API 的初始化
    glfwInit();
    // 配置窗体属性 详见 https://www.glfw.org/docs/latest/window.html#window_hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 主版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 子版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MaxOS 需要

    window = glfwCreateWindow(300, 300, "WINDOW_TITLE", nullptr, nullptr); // 创建并返回窗口的指针
    if (window == nullptr) {
        return 0;
    }
    glfwMakeContextCurrent(window); // 将窗口的上下文设置为当前线程的上下文
    // 给GLAD传入用来加载系统相关的OpenGL函数指针地址的函数
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return 0;
    }

    /*
     * 基本原理是：FrameBuffer -(bind)-> Texture
     * 导出图像上，将 Texture 利用 stb_image 图像库导出为文件；
     * 处理图像上，利用到了 OpenGL 的渲染知识：
     * OpenGL 要渲染一个图像，通常是 draw 到 FrameBuffer 上，如何 draw 到 FrameBuffer 上呢：
     * 一、配置
     * 1. 绑定自己的 FrameBuffer；
     * 2. 指定模型，模型决定了 FrameBuffer 渲染出的图形，它可以是个贴满贴图的盒子，也可以是平面，这里我们只用于处理图片，所以是一个填充满 NDC 坐标的平面；
     * (当然，比如想要的效果是导入的图片贴在一个盒子上，就可以通过修改模型的顶点数据实现。)
     * 3. 加载着色器程序，着色器程序是最终处理像素的程序，由 GPU 完成，这里由它来实现后处理；
     * 顺序没有强关联，只要绘制时都 prepare 即可。
     * 二、渲染
     * 发送数据，完成绘制。
     */
    baseConfig();
    imageRender();
    imageExport();

    // 附 通过窗口上屏
    renderLoop();

    destroy();
    return 0;
}

void baseConfig() {
    importImage();
    bindFrameBuffer();
    loadModel();
    loadShader();
}

void loadShader() {
    shaderProgram = ShaderLoader::loadShader(rectVertexShaderPath, rectFragmentShaderPath);
}

void importImage() {
    Texture import = TextureHelper::genTexture(importImagePath);
    view_port_width = import.width;
    view_port_height = import.height;
    importTexture = import.textureId;
}

// 这里使用自定义的 FrameBuffer，而没有使用窗口的 FrameBuffer，原因是：
// 1. 不需要上屏；
// 2. 方便导出处理后的 Texture。
void bindFrameBuffer() {
    Log::i("bindViewPort", std::to_string(view_port_width) + " " + std::to_string(view_port_height));
    // 视口设置为导入图片的大小，视口和窗口无关，决定了渲染的大小
    glViewport(0, 0, view_port_width, view_port_height);
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view_port_width, view_port_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // 开辟内存 但是不填充数据
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 环绕 使用GL_CLAMP_TO_EDGE是为了防止使用kernel时采样错误
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 添加颜色纹理附件
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0); // ⭐️

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::e("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}

// 平面的顶点数据
static const float rectVertex[] = {
        //     ---- 位置 ----   - 纹理坐标 -
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // 右上
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // 右下
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // 左下
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // 左上
};

// 平面的顶点索引
static const unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

void loadModel() {
    // 配置顶点数据
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertex), rectVertex, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// 图像渲染
void imageRender() {
    // 启用着色器程序
    glUseProgram(shaderProgram);

    // 发送全局数据
    glm::mat4 modelMat(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, importTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0); // 原 Demo 第一个参数写错了！

    // 开始绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

template<typename T>
void invert(T *list, int start, int end) {
    if (start >= end) return;
    for (int i = start; i < end; ++i) {
        if (i >= (end + start) / 2) {
            return;
        }
        T temp = list[i];
        list[i] = list[end - i + start - 1];
        list[end - i + start - 1] = temp;
    }
}

void imageExport() {
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    auto *data = new unsigned char[view_port_width * view_port_height * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // 手动翻转Y轴
    invert(data, 0, view_port_width * view_port_height * 4);
    for (int i = 0; i < view_port_width * view_port_height * 4; i += view_port_width * 4) {
        invert(data, i, i + view_port_width * 4);
    }
    stbi_write_png("/Users/xuzheng/CLionProjects/ImageProcessor/out/write1.png", view_port_width, view_port_height, 4, data, 0);
}

void destroy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
}

void renderLoop() {
    if (window == nullptr) {
        return;
    }
    // 调整视口为窗口需要的大小（注意不是窗口的大小，是窗口需要的大小，在 mac 上视口为窗口的 2 倍）
    glfwGetFramebufferSize(window, &view_port_width, &view_port_height);
    glViewport(0, 0, view_port_width, view_port_height);
    // 之前绑定的是自定义的 FrameBuffer，现在切回默认的（即窗口的）
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        // esc 关闭窗口响应
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // 检查有没有触发什么事件 这个函数必须有
        glfwPollEvents();

        // 清除缓存
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 方便调试环境光影响物体的颜色 纯黑
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // 类似这种函数被成为状态设置函数

        // 渲染
        imageRender();

        glfwSwapBuffers(window); // 交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲）
    }
}
