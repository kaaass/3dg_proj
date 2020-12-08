#include "plane.h"
#include "vertices_data.h"

#include <glad/glad.h>
#include <lighting.h>
#include <game.h>
#include <texture.h>

void Plane::init() {
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glBindVertexArray(0);
}

void Plane::draw(Shader *shader) {
    Lighting::getDefault()->useShader(shader);

    // 视图、投影矩阵
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    auto camera = Game::curStage()->getCamera();
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3{0, 0.49, -3});

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", modelMat);

    shader->setInt("material.texture_diffuse1", 0);
    shader->setInt("material.texture_specular1", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("road"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::of("road_specular"));
    shader->setFloat("material.shininess", 16.0f);

    // 绘制
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
