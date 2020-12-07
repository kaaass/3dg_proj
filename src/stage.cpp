#include "stage.h"
#include "game.h"
#include "texture.h"
#include "lighting.h"

void Stage::prepareDraw() {
    // OpenGL 功能配置
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);

    // 着色器
    standardShader = new Shader("shader/standard.vert", "shader/standard.frag");
    glassShader = new Shader("shader/standard.vert", "shader/glass.frag");
    mirrorShader = new Shader("shader/standard.vert", "shader/mirror.frag");
    modelShaders[0] = new Shader("shader/calc_norm.vert", "shader/calc_norm.geom", "shader/standard.frag");
    modelShaders[1] = modelShaders[0];
    modelShaders[2] = new Shader("shader/calc_norm.vert", "shader/calc_norm.geom", "shader/mirror.frag");
    modelShaders[3] = new Shader("shader/calc_norm.vert", "shader/calc_norm.geom", "shader/glass.frag");

    // 照相机
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // 光照
    Lighting::getDefault()->init(4);
    for (int i = 0; i < 4; i++) {
        Lighting::getDefault()->pointPos(i, glm::vec3{(i - 1.5) * 3, 0.5, -2});
    }

    // 雪花
    snow = new SnowManager;
    snow->init(100);

    // 球体
    for (int i = 0; i < 4; i++) {
        spheres.emplace_back(glm::vec3{(i - 1.5) * 3, 1, 0});
    }

    // 天空盒
    skybox = new Skybox;
    skybox->init();

    // 模型
    model = new Model("model/lucy.obj");
}

void Stage::idle(float delta) {
    // 雪花
    snow->idle(delta);

    // 光照
    Lighting::getDefault()->idle(delta);

    // 摄像机
    camera->idle(delta);
}

void Stage::drawStaff() {
    // 雪花
    glEnable(GL_CULL_FACE);
    snow->draw();
    glDisable(GL_CULL_FACE);

    // 球体
    glEnable(GL_CULL_FACE);
    // 蹭一个雪花材质
    standardShader->use();
    standardShader->setInt("material.texture_diffuse1", 0);
    standardShader->setInt("material.texture_specular1", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("snow"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::of("snow_specular"));
    standardShader->setFloat("material.shininess", 64.0f);
    spheres[0].draw(standardShader);
    // 木板材质
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("wood"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::of("wood_specular"));
    standardShader->setFloat("material.shininess", 16.0f);
    spheres[1].draw(standardShader);
    // 镜面球
    mirrorShader->use();
    mirrorShader->setVec3("cameraPos", camera->position);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemapTexture());
    spheres[2].draw(mirrorShader);
    // 玻璃球
    glassShader->use();
    glassShader->setVec3("cameraPos", camera->position);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemapTexture());
    spheres[3].draw(glassShader);
    //
    glDisable(GL_CULL_FACE);

    // 模型
    drawModels();

    // 天空盒
    skybox->drawLast();
}

Camera *Stage::getCamera() const {
    return camera;
}

void Stage::drawModels() {
    glEnable(GL_CULL_FACE);
    for (int i = 0; i < modelShaders.size(); i++) {
        auto shader = modelShaders[i];
        // 光照
        Lighting::getDefault()->useShader(shader);
        // 各类矩阵
        glm::mat4 projection;
        auto &screen = Game::getInstance()->screen;
        projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                      100.0f);
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3{(i - 1.5) * 3 - 0.4, 0, -4});
        modelMat = glm::scale(modelMat, glm::vec3(1.0) * 0.015f);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("model", modelMat);
        // 着色器设置
        if (i <= 1) {
            // 贴图
            shader->setInt("material.texture_diffuse1", 0);
            shader->setInt("material.texture_specular1", 1);
            if (i == 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Texture::of("snow"));
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Texture::of("snow_specular"));
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Texture::of("wood"));
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Texture::of("wood_specular"));
            }
            shader->setFloat("material.shininess", 64.0f);
        } else {
            shader->setVec3("cameraPos", camera->position);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemapTexture());
        }
        // 绘制
        model->draw(*shader);
    }
    glDisable(GL_CULL_FACE);
}
