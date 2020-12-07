#ifndef INC_3DG_PROJ_STAGE_H
#define INC_3DG_PROJ_STAGE_H

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "snowmanager.h"
#include "sphere.h"

class Stage {
    Shader *shader = nullptr;
    Shader *standardShader = nullptr;

    Camera *camera = nullptr;
    Model *model = nullptr;

    SnowManager * snow = nullptr;

    std::vector<Sphere> spheres;

    uint floorTexture;

public:

    Stage() = default;

    void prepareDraw();

    void idle(float delta);

    void drawStaff();

    [[nodiscard]] Camera *getCamera() const;
};


#endif //INC_3DG_PROJ_STAGE_H
