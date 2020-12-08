#ifndef INC_3DG_PROJ_PLANE_H
#define INC_3DG_PROJ_PLANE_H


#include <shader.h>

class Plane {

    unsigned int planeVAO, planeVBO;

public:

    void init();

    void draw(Shader *shader);

};


#endif //INC_3DG_PROJ_PLANE_H
