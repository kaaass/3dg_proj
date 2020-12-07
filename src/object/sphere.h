#ifndef INC_3DG_PROJ_SPHERE_H
#define INC_3DG_PROJ_SPHERE_H


#include "shader.h"

class Sphere {
    glm::vec3 pos;

    float radius;
    int sectorCount;
    int stackCount;

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    std::vector<float> interleavedVertices;
public:

    explicit Sphere(glm::vec3 pos, float radius = 1.0f, int sectorCount = 36, int stackCount = 18);

    void draw(Shader *shader);

private:
    void buildVertices();

    void updateBuffer();

    void createBuffer();

    void clearArrays();

    void addVertex(float x, float y, float z);

    void addNormal(float x, float y, float z);

    void addTexCoord(float s, float t);

    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
};

#endif //INC_3DG_PROJ_SPHERE_H
