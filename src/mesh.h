#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <assimp/postprocess.h>

struct ModelVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct ModelTexture {
    unsigned int id;
    std::string type;
    aiString path;
};

/**
 * 模型网格类
 */
class Mesh {
public:

    /*  网格数据  */

    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<ModelTexture> textures;

    /*  函数  */

    Mesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices,
         std::vector<ModelTexture> textures);

    void draw(Shader shader);

    [[nodiscard]] GLuint getVaoName() const;

private:

    /*  渲染数据  */

    unsigned int VAO{}, VBO{}, EBO{};

    /*  函数  */

    void setupMesh();
};

#endif