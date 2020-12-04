#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "shader.h"
#include "mesh.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/**
 * 模型加载类
 */
class Model {
public:

    /*  模型数据  */

    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    /*  函数   */

    explicit Model(const std::string &path);

    void draw(Shader shader);

private:

    /*  函数   */

    void loadModel(const std::string& path);

    void processNode(aiNode *node, const aiScene *scene, int layerCnt = 0);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              const std::string& typeName);
};

#endif