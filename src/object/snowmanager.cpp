#include "snowmanager.h"
#include "game.h"
#include "vertex_snow.h"
#include "texture.h"
#include "lighting.h"

#define R_ACC 100
#define R_0_1 ((float)(rand() % R_ACC)/R_ACC)

void SnowManager::init(int pN) {
    // 初始化 shader
    shader = new Shader("shader/snow.vert", "shader/standard.frag");
    // 初始化材质
    shader->use();
    shader->setInt("material.texture_diffuse1", 0);
    shader->setInt("material.texture_specular1", 1);
    // 初始化顶点
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertVBO);
    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_snow), vert_snow, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    // 实例化
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    auto vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 初始化雪花
    resetSnow(pN);
}

void SnowManager::resetSnow(int pN) {
    n = pN;
    snows.clear();
    srand(time(nullptr));
    for (int i = 0; i < n; ++i) {
        Snow sn{};
        sn.x = R_0_1 * ZOOM - (ZOOM / 2);
        sn.y = R_0_1 * ZOOM - (ZOOM / 2);
        sn.z = R_0_1 * ZOOM - (ZOOM / 2);
        sn.r = R_0_1 * 3.1415f / 6;
        sn.v_xmov = -0.0001 * (R_0_1 + 0.1);
        sn.v_ymov = -0.002 * (R_0_1 + 0.5);
        sn.v_zmov = -0.0001 * (R_0_1 + 0.1);
        sn.v_rot = 0.01;
        sn.rot_x = R_0_1 - .5;
        sn.rot_y = R_0_1 - .5;
        sn.rot_z = R_0_1 - .5;
        snows.push_back(sn);
    }
}

void SnowManager::draw() {
    // 模型矩阵
    auto *modelMatrices = new glm::mat4[n * 6];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 6; j++) {
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, glm::vec3{snows[i].x, snows[i].y, snows[i].z});
            modelMat = glm::scale(modelMat, glm::vec3(1) * S_SIZE);
            modelMat = glm::rotate(modelMat, snows[i].r, glm::vec3{snows[i].rot_x, snows[i].rot_y, snows[i].rot_z});
            modelMat = glm::rotate(modelMat, 3.1415f / 3 * j, glm::vec3{0.0, 0.0, -1.0});
            modelMatrices[i * 6 + j] = modelMat;
        }
    }
    // 顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, n * 6 * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] modelMatrices;

    // 光照
    Lighting::getDefault()->useShader(shader);

    // 材质
    shader->setFloat("material.shininess", 64.0f);

    // 视图、投影矩阵
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    auto camera = Game::curStage()->getCamera();
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("snow"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::of("snow_specular"));
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36 * 6, n * 6);
}

void SnowManager::idle(float d) {
    d *= 50;
    for (int i = 0; i < n; i++) {
        snows[i].x += snows[i].v_xmov * d;
        snows[i].y += snows[i].v_ymov * d;
        snows[i].z += snows[i].v_zmov * d;
        snows[i].r += snows[i].v_rot * d;
        if (snows[i].x < -(ZOOM / 2))
            snows[i].x = (ZOOM / 2);
        if (snows[i].y < -(ZOOM / 2))
            snows[i].y = (ZOOM / 2);
    }
}
