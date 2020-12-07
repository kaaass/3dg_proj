#ifndef INC_3DG_PROJ_SKYBOX_H
#define INC_3DG_PROJ_SKYBOX_H

#include <shader.h>

/**
 * 天空盒
 */
class Skybox {

    Shader * shader = nullptr;

    uint VAO;
    uint VBO;

    uint cubemapTexture;

public:

    void init();

    void drawLast();

    uint getCubemapTexture() const;

};


#endif //INC_3DG_PROJ_SKYBOX_H
