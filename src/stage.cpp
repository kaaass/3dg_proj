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
    depthShader = new Shader("shader/depth.vert", "shader/empty.frag");
    mmdShader = new Shader("shader/standard.vert", "shader/mmd.frag");
    prepareShadow();

    // 照相机
    camera = new Camera(glm::vec3(0.0f, 3.0f, 8.0f));

    // 光照
    Lighting::getDefault()->init(4);
    for (int i = 0; i < 4; i++) {
        Lighting::getDefault()->pointPos(i, glm::vec3{(i - 1.5) * 3, 0.5, -2});
    }

    // 地板
    plane = new Plane;
    plane->init();

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
    mmdModel = new Model("model/rin/Black.pmx");

    // 文本
    text = new Text;
    text->init();
}

void Stage::idle(float delta) {
    // 雪花
    snow->idle(delta);

    // 光照
    Lighting::getDefault()->idle(delta);

    // 摄像机
    camera->idle(delta);

    // 文本
    text->idle(delta);
}

void Stage::drawStaff() {
    // 阴影
    drawShadow();
    glViewport(0, 0, Game::getInstance()->screen.width, Game::getInstance()->screen.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 地板
    plane->draw(standardShader);

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("silver"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::of("silver_specular"));
    mirrorShader->use();
    mirrorShader->setInt("material.texture_diffuse1", 0);
    mirrorShader->setInt("material.texture_specular1", 1);
    mirrorShader->setInt("skybox", 5);
    mirrorShader->setVec3("cameraPos", camera->position);
    mirrorShader->setFloat("material.shininess", 64.0f);
    glActiveTexture(GL_TEXTURE5);
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

    // 文本
    text->draw();
}

Camera *Stage::getCamera() const {
    return camera;
}

void Stage::drawModels() {
    glEnable(GL_CULL_FACE);
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();
    for (int i = 0; i < modelShaders.size(); i++) {
        auto shader = modelShaders[i];
        // 光照
        Lighting::getDefault()->useShader(shader);
        // model 矩阵
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
            if (i == 2) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Texture::of("silver"));
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Texture::of("silver_specular"));
                shader->use();
                shader->setInt("material.texture_diffuse1", 0);
                shader->setInt("material.texture_specular1", 1);
                shader->setInt("skybox", 5);
                shader->setVec3("cameraPos", camera->position);
                shader->setFloat("material.shininess", 64.0f);
            } else {
                shader->setVec3("cameraPos", camera->position);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getCubemapTexture());
            }
        }
        // 绘制
        model->draw(*shader);
    }
    // MMD
    Lighting::getDefault()->useShader(mmdShader);
    // model 矩阵
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3{0, 0, 3});
    modelMat = glm::scale(modelMat, glm::vec3(1.0 / 6));
    mmdShader->setMat4("view", view);
    mmdShader->setMat4("projection", projection);
    mmdShader->setMat4("model", modelMat);
    // 绘制
    mmdShader->setFloat("material.shininess", 8.0f);
    mmdModel->draw(*mmdShader);
    glDisable(GL_CULL_FACE);
}

SnowManager *Stage::getSnow() const {
    return snow;
}

void Stage::drawShadow() {
    GLfloat near_plane = 1.0f, far_plane = 40.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(-Lighting::getDefault()->getDirection() * 20.0f,
                                      glm::vec3(0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    depthShader->use();
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // 绘制需要阴影的场景
    plane->draw(depthShader);
    for (auto &sphere : spheres) {
        sphere.draw(depthShader);
    }
    // 模型
    for (int i = 0; i < modelShaders.size(); i++) {
        // model 矩阵
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3{(i - 1.5) * 3 - 0.4, 0, -4});
        modelMat = glm::scale(modelMat, glm::vec3(1.0) * 0.015f);
        depthShader->setMat4("model", modelMat);
        // 绘制
        model->draw(*depthShader);
    }
    // MMD
    // model 矩阵
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3{0, 0, 3});
    modelMat = glm::scale(modelMat, glm::vec3(1.0 / 6));
    depthShader->setMat4("model", modelMat);
    // 绘制
    depthShader->setFloat("material.shininess", 8.0f);
    mmdModel->draw(*depthShader);
    // 结束绘制

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 设置相关参数
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    Shader *shaderUpdate[] = {
            standardShader,
            modelShaders[0], modelShaders[1], modelShaders[2],
            mmdShader,
            mirrorShader
    };
    for (auto shader : shaderUpdate) {
        shader->use();
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader->setInt("shadowMap", 6);
    }
}

void Stage::prepareShadow() {
    // Frame buffer
    glGenFramebuffers(1, &depthMapFBO);
    // Create 2D texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // Bind to depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
