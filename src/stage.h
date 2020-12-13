#ifndef INC_3DG_PROJ_STAGE_H
#define INC_3DG_PROJ_STAGE_H

#include <object/skybox.h>
#include <object/text.h>
#include <object/plane.h>
#include <object/mmd/mmdmodel.h>
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "object/snowmanager.h"
#include "object/sphere.h"

class Stage {
    Shader *standardShader = nullptr;
    Shader *glassShader = nullptr;
    Shader *mirrorShader = nullptr;
    std::vector<Shader *> modelShaders{nullptr, nullptr, nullptr, nullptr};
    Shader *depthShader = nullptr;
    Shader *mmdShader = nullptr;

    Camera *camera = nullptr;
    Model *model = nullptr;
    MmdModel *mmdModel = nullptr;

    SnowManager *snow = nullptr;

    std::vector<Sphere> spheres;

    Skybox *skybox = nullptr;

    Text *text = nullptr;

    Plane *plane = nullptr;

    GLuint depthMapFBO;
    GLuint depthMap;

public:

    static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    Stage() = default;

    void prepareDraw();

    void idle(float delta);

    void drawStaff();

    void prepareShadow();

    void drawShadow();

    [[nodiscard]] Camera *getCamera() const;

    virtual ~Stage() {
        delete standardShader;
        delete camera;
        delete model;
        delete snow;
        delete skybox;
        for (auto p: modelShaders)
            delete p;
        delete text;
    }

    SnowManager *getSnow() const;

    MmdModel *getMmdModel() const;

private:

    void drawModels();
};


#endif //INC_3DG_PROJ_STAGE_H
