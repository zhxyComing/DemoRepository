#version 330 core

in vec2 outTexPos;// 插值过的，不一定是顶点着色器里的outTexPos设置的数值

out vec4 color;
uniform sampler2D texture_diffuse;

// 实现各种后处理效果

void main() {
    //    vec4 tempColor = texture(texture_diffuse, outTexPos);
    // ⭐️ 为什么上屏的画面和导出的图片略有不一致
    // 如果是用于上屏，则alpha为0要过滤，这样像素点无色才能透明；
    // 如果是用于导出图片，则alpha要带上，这样stb_image才能正常导出透明图。
    //    if (tempColor.a < 0.1){
    //        discard;
    //    }
    //    color = tempColor;

    //    color = vec4(tempColor.r, tempColor.b, tempColor.g, tempColor.a);
    //    color = vec4(tempColor.g, tempColor.b, tempColor.r, tempColor.a);
    //    color = vec4(tempColor.r, tempColor.r, tempColor.r, tempColor.a);

    // 黑白效果
    //    float res = (tempColor.r + tempColor.g + tempColor.b) / 3.0f;
    //    color = vec4(res, res, res, tempColor.a);

    // n宫格效果 注意像素点是不变的 变的是该点的颜色 因此只要改变抽的像素即可实现各种效果
    vec2 uv = outTexPos;
    float splite = 2.0f;// 四宫格
    if (uv.x <= 1/splite){
        uv.x = uv.x * splite;
    } else {
        uv.x = (uv.x - 1/splite) * splite;
    }
    if (uv.y <= 1/splite) {
        uv.y = uv.y * splite;
    } else {
        uv.y = (uv.y - 1/splite) * splite;
    }
    color = texture(texture_diffuse, uv);
    float res = (color.r + color.g + color.b) / 3.0f;
    color = vec4(res, res, res, color.a);

    /*
    // 利用kernal实现后处理
    float offset = 1.0 / 300.0;// 偏移量可以自定 注意纹理坐标的范围为0～1，所以 1/WINDOW_WIDTH 为一个横向坐标的偏移量
    vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // 左上
    vec2(0.0f, offset), // 正上
    vec2(offset, offset), // 右上
    vec2(-offset, 0.0f), // 左
    vec2(0.0f, 0.0f), // 中
    vec2(offset, 0.0f), // 右
    vec2(-offset, -offset), // 左下
    vec2(0.0f, -offset), // 正下
    vec2(offset, -offset)// 右下
    );

    float kernel[9] = float[](
    // 锐化kernal
    //    -1, -1, -1,
    //    -1, 9, -1,
    //    -1, -1, -1
    // 模糊kernal
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16
    // 边缘检测 高亮了所有的边缘，而暗化了其它部分
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(texture_diffuse, outTexPos.st + offsets[i]));// 纹理坐标为 vec2 st，纹理为 vec4
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++){
        col += sampleTex[i] * kernel[i];
    }

    color = vec4(col, 1.0);
    */
}
