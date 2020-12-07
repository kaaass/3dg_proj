#include <iostream>
#include "lighting.h"
#include "game.h"

Lighting *Lighting::INSTANCE = nullptr;

void Lighting::init(int pPointLightCount) {
    pointLightCount = pPointLightCount;
}

void Lighting::useShader(Shader *shader) {
    auto camera = Game::curStage()->getCamera();
    shader->use();
    shader->setVec3("viewPos", camera->position);
    // 方向光
    shader->setVec3("dirLight.ambient", dirLight.ambient);
    shader->setVec3("dirLight.diffuse", dirLight.diffuse);
    shader->setVec3("dirLight.specular", dirLight.specular);
    shader->setVec3("dirLight.direction", dirLight.direction);
    // 点光源
    shader->setInt("pointCount", pointLightCount);
    for (int i = 0; i < pointLightCount; i++) {
        std::string uni_name = "pointLights[" + std::to_string(i) + "]";
        shader->setVec3(uni_name + ".ambient", pointLight[i].ambient);
        shader->setVec3(uni_name + ".diffuse", pointLight[i].diffuse);
        shader->setVec3(uni_name + ".specular", pointLight[i].specular);
        shader->setVec3(uni_name + ".position", pointLight[i].position);
        shader->setFloat(uni_name + ".constant", pointLight[i].constant);
        shader->setFloat(uni_name + ".linear", pointLight[i].linear);
        shader->setFloat(uni_name + ".quadratic", pointLight[i].quadratic);
    }
    // 聚光灯
    shader->setVec3("spotLight.ambient", spotLight.ambient);
    shader->setVec3("spotLight.diffuse", spotLight.diffuse);
    shader->setVec3("spotLight.specular", spotLight.specular);
    shader->setVec3("spotLight.position", camera->position);
    shader->setVec3("spotLight.direction", camera->front);
    shader->setFloat("spotLight.cutOff", spotLight.cutOff);
    shader->setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    shader->setFloat("spotLight.constant", spotLight.constant);
    shader->setFloat("spotLight.linear", spotLight.linear);
    shader->setFloat("spotLight.quadratic", spotLight.quadratic);
}

Lighting *Lighting::getDefault() {
    if (INSTANCE == nullptr) {
        INSTANCE = new Lighting;
    }
    return INSTANCE;
}

void Lighting::pointPos(int i, glm::vec3 pos) {
    pointLight[i].position = pos;
}

void Lighting::idle(float delta) {
    float height;
    if (dir) {
        dirLight.direction = glm::vec3{cos(angle / 180 * 3.1415), sin(angle / 180 * 3.1415), -0.3f};
        height = -dirLight.direction[1];
        if (height < 0.1)
            height = 0.1;
        dirLight.ambient = glm::vec3{0.2f, 0.2f, 0.2f} * height;
        dirLight.diffuse = glm::vec3{0.5f, 0.5f, 0.5f} * height;
        dirLight.specular = glm::vec3{0.8f, 0.8f, 0.8f} * height;
        angle += delta * 10;
        if (angle > 360.0)
            angle -= 360.0;
    } else {
        dirLight.direction = glm::vec3{-0.2f, -1.0f, -0.3f};
        dirLight.ambient = glm::vec3{0.2f, 0.2f, 0.2f};
        dirLight.diffuse = glm::vec3{0.5f, 0.5f, 0.5f};
        dirLight.specular = glm::vec3{0.8f, 0.8f, 0.8f};
    }
}

bool Lighting::isDir() const {
    return dir;
}

void Lighting::setDir(bool dir) {
    Lighting::dir = dir;
}

glm::vec3 Lighting::getDirection() const {
    return dirLight.direction;
}
