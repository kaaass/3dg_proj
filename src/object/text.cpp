#include "text.h"

#include <glad/glad.h>
#include <cmath>
#include <iostream>
#include <lighting.h>
#include <game.h>
#include <texture.h>
#include <sstream>

void Text::init() {
    // 字体
    atlas = texture_atlas_new(512, 512, 1);
    font = texture_font_new_from_file(atlas, 44, "font/SimHei.ttf");
    // 绑定材质
    glGenTextures(1, &atlas->id);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 着色器
    shader = new Shader("shader/text.vert", "shader/text.frag");
    // 初始化顶点
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    // VBO格式
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    // 恢复
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 更新一次文本
    idle(0);
}

void Text::idle(float delta) {
    static int frameCnt = -1;
    frameCnt++;
    if (frameCnt > 10) {
        frameCnt = 0;
    }
    if (frameCnt != 0) {
        return;
    }
    // 更新文本
    indices.clear();
    buffer.clear();
    float y = 1.0 - 50.0 / Game::getInstance()->screen.height;
    updateText(delta);
    for (auto &line : texts) {
        y -= addLine(y, line);
    }
    // 更新材质
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);
    // Buffer 赋值
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Text::draw() {
    Lighting::getDefault()->useShader(shader);
    glDepthFunc(GL_LEQUAL);
    // 绘制
    float w = Game::getInstance()->screen.width, h = Game::getInstance()->screen.height;
    shader->setVec3("offset", glm::vec3{2 / w, -2 / h, 0});
    shader->setVec3("color", glm::vec3{0, 0, 0});
    shader->setInt("texture", 0);

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, atlas->id);
    glDrawArrays(GL_TRIANGLES, 0, buffer.size() / 5);

    shader->setVec3("offset", glm::vec3{0, 0, 0});
    shader->setVec3("color", glm::vec3{1, 1, 1});
    glDrawArrays(GL_TRIANGLES, 0, buffer.size() / 5);

    glDepthFunc(GL_LESS);
}

float Text::addLine(float height, const std::string &text) {
    size_t i;
    float w = Game::getInstance()->screen.width, h = Game::getInstance()->screen.height;
    float x = 50.0 / w - 1.0, y = height;
    int line_height = 0;
    for (i = 0; i < text.size(); ++i) {
        texture_glyph_t *glyph = texture_font_get_glyph(font, text.c_str() + i);
        if (glyph != nullptr) {
            float kerning = 0.0f;
            if (i > 0) {
                kerning = texture_glyph_get_kerning(glyph, text.c_str() + i - 1);
            }
            x += kerning;
            float x0 = x + glyph->offset_x / w;
            float y0 = y + glyph->offset_y / h;
            float x1 = x0 + glyph->width / w;
            float y1 = y0 - glyph->height / h;
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            // 添加顶点
            float vertices[][5] = {{x0, y0, 0, s0, t0},
                                   {x0, y1, 0, s0, t1},
                                   {x1, y1, 0, s1, t1},

                                   {x0, y0, 0, s0, t0},
                                   {x1, y1, 0, s1, t1},
                                   {x1, y0, 0, s1, t0}};
            for (auto vert : vertices) {
                for (int j = 0; j < 5; j++)
                    buffer.push_back(vert[j]);
            }
            x += glyph->advance_x / w;
            line_height = std::max((int) glyph->height, line_height);
        }
    }
    return (line_height / h) * 1.1f;
}

void Text::updateText(float delta) {
    texts.clear();
    texts.emplace_back("2018级三维图形程序课程设计");
    std::stringstream sstream;
    sstream << "帧率：" << (1 / delta);
    texts.push_back(sstream.str());

    auto camera = Game::curStage()->getCamera();
    sstream.str("");
    std::string mode = "FPS";
    if (camera->mode == Camera::MODE_AUTO)
        mode = "AUTO";
    else if (camera->mode == Camera::MODE_DRAG)
        mode = "DRAG";
    sstream << "摄像机模式（按C切换）：" << mode;
    texts.push_back(sstream.str());

    sstream.str("");
    sstream << "摄像机位置：[ " << camera->position[0] << ", " << camera->position[1] << ", " << camera->position[2] << "]";
    texts.push_back(sstream.str());

    sstream.str("");
    sstream << "摄像机前向：[ " << camera->front[0] << ", " << camera->front[1] << ", " << camera->front[2] << "]";
    texts.push_back(sstream.str());

    sstream.str("");
    auto dir = Lighting::getDefault()->getDirection();
    sstream << "方向光（按L开关）：" << Lighting::getDefault()->isDir()
        << "；方向：[ " << dir[0] << ", " << dir[1] << ", " << dir[2] << "]";
    texts.push_back(sstream.str());

    sstream.str("");
    sstream << "雪花材质（按T开关）：" << Game::curStage()->getSnow()->isTexture();

    auto &control = Game::getInstance()->control;
    sstream.str("");
    sstream << "动画进度（按R重置P" << (control.anim ? "暂停": "播放") << "）：" << control.animTime;
    texts.push_back(sstream.str());
}
