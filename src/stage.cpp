#include "stage.h"
#include "vertex_data_textures.h"
#include "vertices_data.h"
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
    lightingShader = new Shader("shader/model.vert", "shader/model.frag");
    shader = new Shader("shader/advanced_lighting.vert", "shader/advanced_lighting.frag");
    normalDisplayShader =
            new Shader("shader/normalDisplayShader.vert",
                       "shader/normalDisplayShader.geom",
                       "shader/normalDisplayShader.frag");
    // Create camera
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // shader
    shader->use();
    shader->setInt("floorTexture", 0);

    // 雪花
    snow = new SnowManager;
    snow->init(100);
}

void Stage::idle(float delta) {
    // 雪花
    snow->idle(delta);
}

void Stage::drawStaff() {
    // Projection matrix
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.0f);

    // Lighting
    lightingShader->use();
    float dirAmbient = 1.0f;
    float pointAmbient = dirAmbient;
    float spotAmbient = dirAmbient;

    float dirDiffuse = 0.0f;
    float pointDiffuse = dirDiffuse;
    float spotDiffuse = dirDiffuse;

    float dirSpec = 0.0f;
    float pointSpec = dirSpec;
    float spotSpec = dirSpec;

    lightingShader->setVec3("viewPos", camera->position);

    lightingShader->setVec3("dirLight.ambient",  dirAmbient, dirAmbient, dirAmbient);
    lightingShader->setVec3("dirLight.diffuse",  dirDiffuse, dirDiffuse, dirDiffuse);
    lightingShader->setVec3("dirLight.specular", dirSpec, dirSpec, dirSpec);
    lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    lightingShader->setInt("pointCount", pointLightPositions.size());
    for (int i = 0; i < 4; i++) {
        std::string uni_name = "pointLights[" + std::to_string(i) + "]";
        lightingShader->setVec3(uni_name + ".ambient",  pointAmbient, pointAmbient, pointAmbient);
        lightingShader->setVec3(uni_name + ".diffuse",  pointDiffuse, pointDiffuse, pointDiffuse);
        lightingShader->setVec3(uni_name + ".specular", pointSpec, pointSpec, pointSpec);
        lightingShader->setVec3(uni_name + ".position", pointLightPositions[i]);
        lightingShader->setFloat(uni_name + ".constant",  1.0f);
        lightingShader->setFloat(uni_name + ".linear",    0.09f);
        lightingShader->setFloat(uni_name + ".quadratic", 0.032f);
    }

    lightingShader->setVec3("spotLight.ambient",  spotAmbient, spotAmbient, spotAmbient);
    lightingShader->setVec3("spotLight.diffuse",  spotDiffuse, spotDiffuse, spotDiffuse);
    lightingShader->setVec3("spotLight.specular", spotSpec, spotSpec, spotSpec);
    lightingShader->setVec3("spotLight.position",  camera->position);
    lightingShader->setVec3("spotLight.direction", camera->front);
    lightingShader->setFloat("spotLight.cutOff",   glm::cos(glm::radians(12.5f)));
    lightingShader->setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(17.5f)));
    lightingShader->setFloat("spotLight.constant",  1.0f);
    lightingShader->setFloat("spotLight.linear",    0.09f);
    lightingShader->setFloat("spotLight.quadratic", 0.032f);

    // Material
    lightingShader->setFloat("material.shininess", 64.0f);

    // floor
    lightingShader->setMat4("view", view);
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("model", modelMat);

//    glBindVertexArray(planeVAO);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, floorTexture);
//    glDrawArrays(GL_TRIANGLES, 0, 36*6);
    snow->draw();

    normalDisplayShader->use();
    normalDisplayShader->setMat4("projection", projection);
    normalDisplayShader->setMat4("view", view);
    normalDisplayShader->setMat4("model", modelMat);

    glDrawArrays(GL_TRIANGLES, 0, 36*6);
}

Camera *Stage::getCamera() const {
    return camera;
}
