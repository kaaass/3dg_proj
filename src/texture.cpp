#include <iostream>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int TextureLoader::textureFromFile(std::string filepath, std::string directory) {
    int pos = 0;
    // Win路径修正
    while ((pos = filepath.find("\\")) >= 0) {
        filepath.replace(pos, 1, "/");
    }
    filepath = directory + "/" + filepath;
    return loadTexture(filepath);
}

unsigned int TextureLoader::loadTexture(std::string filepath, int warp_s, int warp_t, bool gammaCorrection) {
    unsigned int texture = 0;
    std::cout << " :- - Start loading texture: " << filepath << std::endl;
    // Create texture
    glGenTextures(1, &texture);
    // Load and generate texture
    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    GLenum mode;
    GLenum internal;
    if (data) {
        if (nrChannels == 1) {
            internal = mode = GL_RED;
        } else if (nrChannels == 3) {
            internal = gammaCorrection ? GL_SRGB : GL_RGB;
            mode = GL_RGB;
        } else if (nrChannels == 4) {
            internal = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            mode = GL_RGBA;
            // std::cout << "Alpha enabled for: " << filepath << std::endl;
        }
        // std::cout << nrChannels << std::endl;
        glBindTexture(GL_TEXTURE_2D, texture);
        // Load data
        glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // Configure wrap and filter type
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // Free image
    stbi_image_free(data);
    return texture;
}

unsigned int TextureLoader::loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    std::cout << "Start loading cubemap: " << faces[0] << ", ..." << std::endl;

    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

std::map<std::string, unsigned int> TextureLoader::TEXTURE_CACHE;

unsigned int TextureLoader::of(const std::string& id) {
    if (TEXTURE_CACHE.count(id) <= 0) {
        TEXTURE_CACHE[id] = loadTexture("image/" + id + ".png");
    }
    return TEXTURE_CACHE[id];
}
