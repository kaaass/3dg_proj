#include "shader.h"

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    ID = vertex = geometry = fragment = 0;
    // 读入着色器文件内容
    std::string vertexCode = readShaderFromFile(vertexPath);
    std::string fragmentCode = readShaderFromFile(fragmentPath);
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 编译着色器程序
    vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
    fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
    // 链接
    linkShaderProgram();
}

Shader::Shader(const char *vertexPath, const char *geometryPath, const char *fragmentPath) {
    ID = vertex = geometry = fragment = 0;
    // 读入着色器文件内容
    std::string vertexCode = readShaderFromFile(vertexPath);
    std::string geometryCode = readShaderFromFile(geometryPath);
    std::string fragmentCode = readShaderFromFile(fragmentPath);
    const char *vShaderCode = vertexCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 编译着色器程序
    vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
    geometry = compileShader(gShaderCode, GL_GEOMETRY_SHADER);
    fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
    // 链接
    linkShaderProgram();
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec2(const std::string &name, glm::vec2 value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float f1, float f2, float f3) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), f1, f2, f3);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

std::string Shader::readShaderFromFile(const char *shaderPath) {
    std::ifstream shaderFile;
    // 允许错误
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // 读入文件内容
        shaderFile.open(shaderPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return vShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    return "unknown";
}

unsigned int Shader::compileShader(const char *shaderCode, unsigned int type) {
    unsigned int result = glCreateShader(type);
    glShaderSource(result, 1, &shaderCode, nullptr);
    glCompileShader(result);
    std::string str_type;
    switch (type) {
        case GL_VERTEX_SHADER:
            str_type = "VERTEX";
            break;
        case GL_GEOMETRY_SHADER:
            str_type = "GEOMETRY";
            break;
        case GL_FRAGMENT_SHADER:
            str_type = "FRAGMENT";
            break;
        default:
            str_type = "UNKNOWN";
    }
    checkCompileErrors(result, str_type);
    return result;
}

unsigned int Shader::linkShaderProgram() {
    ID = glCreateProgram();
    if (vertex > 0) glAttachShader(ID, vertex);
    if (geometry > 0) glAttachShader(ID, geometry);
    if (fragment > 0) glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    if (vertex > 0) glDeleteShader(vertex);
    if (geometry > 0) glDeleteShader(geometry);
    if (fragment > 0) glDeleteShader(fragment);
    vertex = geometry = fragment = 0;
    return ID;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
