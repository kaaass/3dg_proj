#ifndef INC_3DG_PROJ_MMDMATERIAL_H
#define INC_3DG_PROJ_MMDMATERIAL_H

struct Material {
    explicit Material(const saba::MMDMaterial *mat)
            : m_mmdMat(mat) {}

    const saba::MMDMaterial *m_mmdMat;
    GLuint m_texture = 0;
    GLuint m_spTexture = 0;
    GLuint m_toonTexture = 0;
};

#endif //INC_3DG_PROJ_MMDMATERIAL_H
