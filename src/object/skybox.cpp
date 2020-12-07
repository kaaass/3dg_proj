#include "skybox.h"

#include <glad/glad.h>
#include <game.h>
#include <object/vertices_data.h>
#include <texture.h>

void Skybox::init() {
    // 顶点
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glBindVertexArray(0);

    // 材质
    std::vector<std::string> faces {
            "image/sky/right.jpg",
            "image/sky/left.jpg",
            "image/sky/top.jpg",
            "image/sky/bottom.jpg",
            "image/sky/front.jpg",
            "image/sky/back.jpg"
    };
    cubemapTexture = Texture::loadCubemap(faces);

    // 着色器
    shader = new Shader("shader/skybox.vert", "shader/skybox.frag");

}

void Skybox::drawLast() {
    shader->use();

    // 视图、投影矩阵
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    auto camera = Game::curStage()->getCamera();
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // 绘制
    glDepthFunc(GL_LEQUAL);
    shader->use();
    shader->setMat4("view", glm::mat4(glm::mat3(view)));
    shader->setMat4("projection", projection);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}
