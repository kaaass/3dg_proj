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
    shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
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
