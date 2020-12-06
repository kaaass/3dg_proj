#ifndef INC_3DG_PROJ_STAGE_H
#define INC_3DG_PROJ_STAGE_H

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "snowmanager.h"

class Stage {
    Shader *lightingShader = nullptr;
    Shader *lampShader = nullptr;
    Shader *shader = nullptr;
    Shader* normalDisplayShader = nullptr;

    Camera *camera = nullptr;
    Model *model = nullptr;

    unsigned int VBO;
    unsigned int planeVBO;
    unsigned int lightVAO;
    unsigned int planeVAO;
    unsigned int floorTexture;

    SnowManager * snow = nullptr;

    glm::vec3 lightPos;

    std::vector<glm::vec3> pointLightPositions = {
            glm::vec3(3.7f, 3.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f)
    };

public:

    Stage(): lightPos(0.0f, 0.0f, 0.0f) { }

    void prepareDraw();

    void idle(float delta);

    void drawStaff();

    [[nodiscard]] Camera *getCamera() const;
};


#endif //INC_3DG_PROJ_STAGE_H
