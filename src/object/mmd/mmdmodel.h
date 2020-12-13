#ifndef INC_3DG_PROJ_MMDMODEL_H
#define INC_3DG_PROJ_MMDMODEL_H


#include <string>
#include <shader.h>
#include <glad/glad.h>
#include <Saba/Base/Path.h>
#include <Saba/Base/File.h>
#include <Saba/Base/UnicodeUtil.h>
#include <Saba/Base/Time.h>
#include <Saba/Model/MMD/PMDModel.h>
#include <Saba/Model/MMD/PMXModel.h>
#include <Saba/Model/MMD/VMDFile.h>
#include <Saba/Model/MMD/VMDAnimation.h>
#include <Saba/Model/MMD/VMDCameraAnimation.h>
#include "mmdmaterial.h"

class MmdModel {

    std::shared_ptr<saba::MMDModel> m_mmdModel;
    std::unique_ptr<saba::VMDAnimation>	m_vmdAnim;

    GLuint m_posVBO = 0;
    GLuint m_norVBO = 0;
    GLuint m_uvVBO = 0;
    GLuint m_ibo = 0;
    GLenum m_indexType;
    GLuint m_mmdVAO = 0;

    std::vector<Material> m_materials;

public:

    explicit MmdModel(const std::string &modelPath, const std::string &animPath);

    void init();

    void draw(Shader *shader);

    void idle(float delta);
    
    void resetAnim();
};


#endif //INC_3DG_PROJ_MMDMODEL_H
