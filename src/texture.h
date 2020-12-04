#ifndef INC_3DG_PROJ_TEXTURE_H
#define INC_3DG_PROJ_TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <vector>

class TextureLoader {
public:
    static unsigned int textureFromFile(std::string filepath, std::string directory);

    static unsigned int loadTexture(std::string filepath, int warp_s = GL_REPEAT, int warp_t = GL_REPEAT, bool gammaCorrection = false);

    static unsigned int loadCubemap(std::vector<std::string> faces);

};


#endif //INC_3DG_PROJ_TEXTURE_H
