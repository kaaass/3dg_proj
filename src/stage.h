#ifndef INC_3DG_PROJ_STAGE_H
#define INC_3DG_PROJ_STAGE_H

#include <object/skybox.h>
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

    Camera *camera = nullptr;
    Model *model = nullptr;

    SnowManager *snow = nullptr;

    std::vector<Sphere> spheres;

    Skybox *skybox = nullptr;

public:

    Stage() = default;

    void prepareDraw();

    void idle(float delta);

    void drawStaff();

    [[nodiscard]] Camera *getCamera() const;

    virtual ~Stage() {
        delete standardShader;
        delete camera;
        delete model;
        delete snow;
        delete skybox;
        for (auto p: modelShaders)
            delete p;
    }

private:

    void drawModels();
};


#endif //INC_3DG_PROJ_STAGE_H
