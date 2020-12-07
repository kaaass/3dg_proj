#ifndef INC_3DG_PROJ_TEXTURE_H
#define INC_3DG_PROJ_TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <map>

class TextureLoader {

    static std::map<std::string, unsigned int> TEXTURE_CACHE;

public:
    static unsigned int of(const std::string& id);

    static unsigned int textureFromFile(std::string filepath, std::string directory);

    static unsigned int loadTexture(std::string filepath, int warp_s = GL_REPEAT, int warp_t = GL_REPEAT, bool gammaCorrection = false);

    static unsigned int loadCubemap(std::vector<std::string> faces);

};


#endif //INC_3DG_PROJ_TEXTURE_H
