#include "stage.h"
#include "game.h"
#include "texture.h"
#include "lighting.h"

void Stage::prepareDraw() {
    // OpenGL 功能配置
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 着色器
    standardShader = new Shader("shader/standard.vert", "shader/standard.frag");

    // 照相机
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // 光照
    Lighting::getDefault()->init(0);

    // 雪花
    snow = new SnowManager;
    snow->init(100);

    // 球体
    for (int i = 0; i < 2; i++) {
        spheres.emplace_back(glm::vec3{i * 3, 0, 0});
    }

    // 天空盒
    skybox = new Skybox;
    skybox->init();
}

void Stage::idle(float delta) {
    // 雪花
    snow->idle(delta);
}

void Stage::drawStaff() {
    // 雪花
    snow->draw();

    // 球体
    // 蹭一个雪花材质
    standardShader->use();
    standardShader->setFloat("material.shininess", 64.0f);
    spheres[0].draw(standardShader);
    // 木板材质
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::of("wood"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    standardShader->setFloat("material.shininess", 16.0f);
    spheres[1].draw(standardShader);

    // 天空盒
    skybox->drawLast();
}

Camera *Stage::getCamera() const {
    return camera;
}
