#include "stage.h"
#include "vertex_data_textures.h"
#include "vertices_data.h"
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

    // Create shader
    lightingShader = new Shader("shader/model.vert", "shader/model.frag");
    shader = new Shader("shader/advanced_lighting.vert", "shader/advanced_lighting.frag");

    // 照相机
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // 光照
    Lighting::getDefault()->init(0);

    // 雪花
    snow = new SnowManager;
    snow->init(100);
}

void Stage::idle(float delta) {
    // 雪花
    snow->idle(delta);
}

void Stage::drawStaff() {
    // 雪花
    snow->draw();
}

Camera *Stage::getCamera() const {
    return camera;
}
