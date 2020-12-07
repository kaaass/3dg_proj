#ifndef INC_3DG_PROJ_LIGHTING_H
#define INC_3DG_PROJ_LIGHTING_H

#include <glm/glm.hpp>
#include "shader.h"

#define NR_POINT_LIGHTS 10

/**
 * 光照管理
 */
class Lighting {
public:
    struct DirLight {
        glm::vec3 direction{-0.2f, -1.0f, -0.3f};

        glm::vec3 ambient{0.2f, 0.2f, 0.2f};
        glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
        glm::vec3 specular{0.8f, 0.8f, 0.8f};
    };

    struct PointLight {
        glm::vec3 position{3.7f,  3.2f,  2.0f};

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        glm::vec3 ambient{0.1f, 0.1f, 0.1f};
        glm::vec3 diffuse{0.4f, 0.4f, 0.4f};
        glm::vec3 specular{0.6f, 0.6f, 0.6f};
    };

    struct SpotLight {
        glm::vec3 position;
        glm::vec3 direction;
        float cutOff = glm::cos(glm::radians(12.5f));
        float outerCutOff = glm::cos(glm::radians(17.5f));

        glm::vec3 ambient{0.2f, 0.2f, 0.2f};
        glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
        glm::vec3 specular{0.8f, 0.8f, 0.8f};

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };
private:

    DirLight dirLight;
    PointLight pointLight[NR_POINT_LIGHTS];
    SpotLight spotLight;

    int pointLightCount;

    bool dir = true;
    float angle = -60.0f;

    static Lighting *INSTANCE;

public:

    void init(int pPointLightCount);

    void idle(float delta);

    void pointPos(int i, glm::vec3 pos);

    void useShader(Shader *shader);

    static Lighting *getDefault();

    [[nodiscard]] glm::vec3 getDirection() const;

    bool isDir() const;

    void setDir(bool dir);
};


#endif //INC_3DG_PROJ_LIGHTING_H
