#include "stage.h"
#include "vertex_data_textures.h"
#include "game.h"

void Stage::prepareDraw() {
    // OpenGL 功能配置
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create shader
    lightingShader = new Shader("shader/model.vert", "shader/model.frag");
    lampShader = new Shader("shader/colors.vert", "shader/colors_light.frag");

    // Load model
    // model/gennso/gennso.pmx
    // model/MT-MIKU/MT-MIKU.pmx
    model = new Model("/home/kaaass/Project/graphic/learnopengl/src/toy/model/rin/Black.pmx");

    // Create camera
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // Create VBO, VAO
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &lightVAO);

    // Rectangle
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
}

void Stage::drawStaff() {
    lightingShader->use();

    // Projection matrix
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f, 100.0f);

    // Pass the transform matrixs
    lightingShader->setMat4("view", camera->getViewMatrix());
    lightingShader->setMat4("projection", projection);

    // Lighting
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

    lightingShader->setVec3("dirLight.ambient", dirAmbient, dirAmbient, dirAmbient);
    lightingShader->setVec3("dirLight.diffuse", dirDiffuse, dirDiffuse, dirDiffuse);
    lightingShader->setVec3("dirLight.specular", dirSpec, dirSpec, dirSpec);
    lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    lightingShader->setInt("pointCount", pointLightPositions.size());
    for (int i = 0; i < 4; i++) {
        std::string uni_name = "pointLights[" + std::to_string(i) + "]";
        lightingShader->setVec3(uni_name + ".ambient", pointAmbient, pointAmbient, pointAmbient);
        lightingShader->setVec3(uni_name + ".diffuse", pointDiffuse, pointDiffuse, pointDiffuse);
        lightingShader->setVec3(uni_name + ".specular", pointSpec, pointSpec, pointSpec);
        lightingShader->setVec3(uni_name + ".position", pointLightPositions[i]);
        lightingShader->setFloat(uni_name + ".constant", 1.0f);
        lightingShader->setFloat(uni_name + ".linear", 0.09f);
        lightingShader->setFloat(uni_name + ".quadratic", 0.032f);
    }

    lightingShader->setVec3("spotLight.ambient", spotAmbient, spotAmbient, spotAmbient);
    lightingShader->setVec3("spotLight.diffuse", spotDiffuse, spotDiffuse, spotDiffuse);
    lightingShader->setVec3("spotLight.specular", spotSpec, spotSpec, spotSpec);
    lightingShader->setVec3("spotLight.position", camera->position);
    lightingShader->setVec3("spotLight.direction", camera->front);
    lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    lightingShader->setFloat("spotLight.constant", 1.0f);
    lightingShader->setFloat("spotLight.linear", 0.09f);
    lightingShader->setFloat("spotLight.quadratic", 0.032f);

    // Material
    lightingShader->setFloat("material.shininess", 64.0f);

    // Drawing
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat,
                              glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
    modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader->setMat4("model", modelMat);
    model->draw(*lightingShader);

    // Lamp cube
    // lampShader->use();

    // for (int i = 0; i < pointLightPositions.size(); i++) {
    //     modelMat = glm::mat4();
    //     modelMat = glm::translate(modelMat, pointLightPositions[i]);
    //     modelMat = glm::scale(modelMat, glm::vec3(0.2f));
    //     lampShader->setMat4("model", modelMat);
    //     lampShader->setMat4("view", camera->getViewMatrix());
    //     lampShader->setMat4("projection", projection);

    //     glBindVertexArray(lightVAO);
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
}

Camera *Stage::getCamera() const {
    return camera;
}
