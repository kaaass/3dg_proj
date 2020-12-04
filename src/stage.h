#ifndef INC_3DG_PROJ_STAGE_H
#define INC_3DG_PROJ_STAGE_H

#include "shader.h"
#include "camera.h"
#include "model.h"

class Stage {
    Shader *lightingShader = nullptr;
    Shader *lampShader = nullptr;

    Camera *camera = nullptr;
    Model *model = nullptr;

    unsigned int VBO;
    unsigned int lightVAO;

    std::vector<glm::vec3> pointLightPositions = {
            glm::vec3(3.7f, 3.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f)
    };

public:

    void prepareDraw();

    void drawStaff();

    Camera *getCamera() const;
};


#endif //INC_3DG_PROJ_STAGE_H
