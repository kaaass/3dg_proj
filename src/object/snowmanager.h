#ifndef INC_3DG_PROJ_SNOWMANAGER_H
#define INC_3DG_PROJ_SNOWMANAGER_H

#include <vector>
#include "shader.h"

/**
 * 雪花绘制管理
 */
class SnowManager {
public:
    struct Snow {
        float x, y, z, r;
        float v_xmov, v_ymov, v_zmov, v_rot;
        float rot_x, rot_y, rot_z;
    };

private:
    std::vector<Snow> snows;
    std::vector<Snow> generatedSnow;

    Shader *shader = nullptr;

    int n = 0;

    float ZOOM = 10.0;
    float S_SIZE = 0.02;

    uint VAO;
    uint vertVBO;
    uint instanceVBO;

    bool texture = true;
public:
    bool isTexture() const;

    void setTexture(bool texture);

public:

    void init(int n);

    void resetSnow(int n);

    void draw();

    void idle(float d);

    void spawnOnScreen(float x, float y);

    virtual ~SnowManager() { delete shader; }
};


#endif //INC_3DG_PROJ_SNOWMANAGER_H
