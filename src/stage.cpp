#include "stage.h"
#include "vertex_data_textures.h"
#include "vertices_data.h"
#include "vertex_snow.h"
#include "game.h"
#include "texture.h"

void Stage::prepareDraw() {
    // OpenGL 功能配置
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create shader
    shader = new Shader("shader/advanced_lighting.vert", "shader/advanced_lighting.frag");
    normalDisplayShader =
            new Shader("shader/normalDisplayShader.vert",
                       "shader/normalDisplayShader.geom",
                       "shader/normalDisplayShader.frag");
    // Create camera
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_snow), vert_snow, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // Load texture
    floorTexture = TextureLoader::loadTexture("image/wood.png");

    // shader
    shader->use();
    shader->setInt("floorTexture", 0);
}

void Stage::idle(float delta) {

}

void Stage::drawStaff() {
    // Projection matrix
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.0f);

    shader->use();
    shader->setVec3("viewPos", camera->position);
    shader->setVec3("lightPos", lightPos);
    shader->setInt("blinn", true);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36*6);

    normalDisplayShader->use();
    normalDisplayShader->setMat4("projection", projection);
    normalDisplayShader->setMat4("view", view);
    normalDisplayShader->setMat4("model", modelMat);

    glDrawArrays(GL_TRIANGLES, 0, 36*6);
}

Camera *Stage::getCamera() const {
    return camera;
}
