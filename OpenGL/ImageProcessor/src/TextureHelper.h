//
// Created by 徐政 on 2022/1/18.
// 从指定路径加载图片到纹理（纹理数据在显存）
//

#ifndef IMAGEPROCESSOR_TEXTUREHELPER_H
#define IMAGEPROCESSOR_TEXTUREHELPER_H

#include "glad/glad.h"
#include "stb_image.h"
#include "Log.h"
#include "bean/Texture.h"

namespace TextureHelper {

    static Texture genTexture(const char *path) {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 环绕
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 环绕
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 过滤 包含多级纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 过滤
        int width, height, channel;
        // OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部。stb_image.h能够在图像加载时帮助我们翻转y轴。
        stbi_set_flip_vertically_on_load(true);
        stbi_uc *data = stbi_load(path, &width, &height, &channel, 0); // 此时数据在CPU 通过glTexImage2D 发送数据到GPU显存
        Log::i("Dixon_Gen_Texture", std::to_string(channel) + " " + path + " " + std::to_string(width) + " " + std::to_string(height));
        if (data) {
            if (channel == 1) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            } else if (channel == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if (channel == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(data);
        Texture texture = {.textureId = textureId, .width = width, .height = height, .channel = channel};
        return texture;
    }
}

#endif //IMAGEPROCESSOR_TEXTUREHELPER_H
