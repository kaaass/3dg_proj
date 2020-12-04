#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int textureFromFile(string filepath, string directory);

unsigned int loadTexture(string filepath, int warp_s = GL_REPEAT, int warp_t = GL_REPEAT, bool gammaCorrection = false);

class Model {
public:

    /*  模型数据  */

    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;

    /*  函数   */

    Model(char *path) {
        loadModel(path);
    }

    void draw(Shader shader) {
        for (auto mesh : meshes) {
            mesh.draw(shader);
        }
    }

private:

    /*  函数   */

    void loadModel(string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene, int layerCnt = 0) {
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

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
#ifdef PRINT_MESH
        std::cout << " :- mesh: " << mesh->mName.C_Str() << std::endl;
#endif
        for (int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;
            // Vertex
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normal
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
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
            vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                               aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         string typeName) {
        vector<Texture> textures;
        for (int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (auto cache : textures_loaded) {
                if (std::strcmp(cache.path.C_Str(), str.C_Str()) == 0) {
                    textures.push_back(cache);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id = textureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
            }
        }
        return textures;
    }
};

unsigned int textureFromFile(string filepath, string directory) {
    int pos = 0;
    // Win路径修正
    while ((pos = filepath.find("\\")) >= 0) {
        filepath.replace(pos, 1, "/");
    }
    filepath = directory + "/" + filepath;
    return loadTexture(filepath);
}

unsigned int loadTexture(string filepath, int warp_s, int warp_t, bool gammaCorrection) {
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

unsigned int loadCubemap(vector<std::string> faces) {
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

#endif