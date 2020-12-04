#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

/**
 * 着色器管理类
 */
class Shader {
public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);

    Shader(const char *vertexPath, const char *geometryPath, const char *fragmentPath);

    /**
     * 启用着色器
     */
    void use() const;

    /*
     * uniform 设置
     */

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string &name, glm::mat4 value) const;

    void setVec2(const std::string &name, glm::vec2 value) const;

    void setVec3(const std::string &name, float f1, float f2, float f3) const;

    void setVec3(const std::string &name, glm::vec3 value) const;

private:
    unsigned int vertex, fragment, geometry;

    /**
     * 读入着色器文件内容
     */
    static std::string readShaderFromFile(const char *shaderPath);

    /**
     * 编译着色器
     */
    static unsigned int compileShader(const char *shaderCode, unsigned int type);

    /**
     * 链接程序
     */
    unsigned int linkShaderProgram();

    /**
     * 检查着色器编译错误
     */
    static void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif