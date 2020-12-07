#include <cmath>
#include <vector>
#include "sphere.h"
#include "lighting.h"
#include "game.h"
#include <glad/glad.h>

/**
 * 计算球体顶点
 */
void Sphere::buildVertices() {
    const float PI = acos(-1);

    clearArrays();

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // 顶点数据
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;
            // 顶点
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            addVertex(x, y, z);
            // 法向量
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            addNormal(nx, ny, nz);
            // 材质坐标
            s = (float) j / sectorCount;
            t = (float) i / stackCount;
            addTexCoord(s, t);
        }
    }

    // 从顶点构建三角形，计算 indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 处理一个面
            if (i != 0) {
                addIndices(k1, k2, k1 + 1);   // k1、k2、k1+1
            }
            if (i != (stackCount - 1)) {
                addIndices(k1 + 1, k2, k2 + 1); // k1+1、k2、k2+1
            }
            // 线框绘制
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if (i != 0) {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }
}

/**
 * 更新VBO等内容
 */
void Sphere::updateBuffer() {
    std::vector<float>().swap(interleavedVertices);

    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2) {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);

        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j + 1]);
    }
    // Buffer 赋值
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), interleavedVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * 创建缓冲区
 */
void Sphere::createBuffer() {
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    // VBO格式
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    // 恢复
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Sphere::clearArrays() {
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);
    std::vector<unsigned int>().swap(indices);
}

void Sphere::addVertex(float x, float y, float z) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void Sphere::addNormal(float nx, float ny, float nz) {
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}

void Sphere::addTexCoord(float s, float t) {
    texCoords.push_back(s);
    texCoords.push_back(t);
}

void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

Sphere::Sphere(glm::vec3 pos, float radius, int sectorCount, int stackCount)
        : pos(pos), radius(radius), sectorCount(sectorCount), stackCount(stackCount) {
    buildVertices();
    createBuffer();
    updateBuffer();
}

void Sphere::draw(Shader *shader) {
    Lighting::getDefault()->useShader(shader);

    // 视图、投影矩阵
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    auto camera = Game::curStage()->getCamera();
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, pos);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", modelMat);

    // 绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
