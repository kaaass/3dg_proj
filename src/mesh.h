#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <assimp/postprocess.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
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

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    /*  函数  */

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures);

    void draw(Shader shader);

    [[nodiscard]] GLuint getVaoName() const;

private:

    /*  渲染数据  */

    unsigned int VAO{}, VBO{}, EBO{};

    /*  函数  */

    void setupMesh();
};

#endif