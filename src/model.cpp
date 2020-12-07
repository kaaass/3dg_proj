#include "model.h"

#include <iostream>
#include <string>
#include <vector>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
#include "texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const std::string &path) {
    loadModel(path);
}

void Model::draw(Shader shader) {
    for (auto mesh : meshes) {
        mesh.draw(shader);
    }
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene, int layerCnt) {
#ifdef PRINT_NODE
    // 打印Node名称
        for (int i = 0; i < layerCnt; i++)
            std::cout << " -";
        std::cout << " " << node->mName.C_Str() << ", num = " << node->mNumMeshes << std::endl;
#endif
    // 处理节点所有的网格（如果有的话）
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, layerCnt + 1);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<ModelTexture> textures;
#ifdef PRINT_MESH
    std::cout << " :- mesh: " << mesh->mName.C_Str() << std::endl;
#endif
    for (int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex{};
        glm::vec3 vector;
        // Vertex
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // Normal
        if (mesh->mNormals != nullptr) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // Texture
        if (mesh->mTextureCoords[0]) { // 网格是否有纹理坐标？
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // 处理索引
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // 处理材质
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // diffuse maps
        std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(material,
                                                                     aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // specular maps
        std::vector<ModelTexture> specularMaps = loadMaterialTextures(material,
                                                                      aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<ModelTexture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName) {
    std::vector<ModelTexture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (const auto& cache : textures_loaded) {
            if (std::strcmp(cache.path.C_Str(), str.C_Str()) == 0) {
                textures.push_back(cache);
                skip = true;
                break;
            }
        }
        if (!skip) {
            ModelTexture texture;
            texture.id = Texture::textureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
        }
    }
    return textures;
}
