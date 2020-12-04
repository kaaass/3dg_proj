#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"

#include "camera.h"
#include "vertex_data_textures.h"

int screenWidth = 1280, screenHeight = 720;

Shader *lightingShader = nullptr;
Shader *lampShader = nullptr;

unsigned int VBO;
unsigned int lightVAO;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

Camera *camera = nullptr;
Model *model = nullptr;
double lastX = (double) screenWidth / 2;
double lastY = (double) screenHeight / 2;
bool firstMouse = true;

std::vector<glm::vec3> pointLightPositions = {
        glm::vec3(3.7f, 3.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f)
};

void prepareDraw() {
    // OpenGL 功能配置
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create shader
    lightingShader = new Shader("shader/model.vert", "shader/model.frag");
    lampShader = new Shader("shader/colors.vert", "shader/colors_light.frag");

    // Load model
    // model/gennso/gennso.pmx
    // model/MT-MIKU/MT-MIKU.pmx
    model = new Model("/home/kaaass/Project/graphic/learnopengl/src/toy/model/rin/Black.pmx");

    // Create camera
    camera = new Camera(glm::vec3(1.0f, 1.0f, 5.0f));

    // Create VBO, VAO
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &lightVAO);

    // Rectangle
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
}

void drawStaff() {
    lightingShader->use();

    // Projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera->zoom), (double) screenWidth / screenHeight, 0.1, 100.0);

    // Pass the transform matrixs
    lightingShader->setMat4("view", camera->getViewMatrix());
    lightingShader->setMat4("projection", projection);

    // Lighting
    float dirAmbient = 1.0f;
    float pointAmbient = dirAmbient;
    float spotAmbient = dirAmbient;

    float dirDiffuse = 0.0f;
    float pointDiffuse = dirDiffuse;
    float spotDiffuse = dirDiffuse;

    float dirSpec = 0.0f;
    float pointSpec = dirSpec;
    float spotSpec = dirSpec;

    lightingShader->setVec3("viewPos", camera->position);

    lightingShader->setVec3("dirLight.ambient", dirAmbient, dirAmbient, dirAmbient);
    lightingShader->setVec3("dirLight.diffuse", dirDiffuse, dirDiffuse, dirDiffuse);
    lightingShader->setVec3("dirLight.specular", dirSpec, dirSpec, dirSpec);
    lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    lightingShader->setInt("pointCount", pointLightPositions.size());
    for (int i = 0; i < 4; i++) {
        std::string uni_name = "pointLights[" + std::to_string(i) + "]";
        lightingShader->setVec3(uni_name + ".ambient", pointAmbient, pointAmbient, pointAmbient);
        lightingShader->setVec3(uni_name + ".diffuse", pointDiffuse, pointDiffuse, pointDiffuse);
        lightingShader->setVec3(uni_name + ".specular", pointSpec, pointSpec, pointSpec);
        lightingShader->setVec3(uni_name + ".position", pointLightPositions[i]);
        lightingShader->setFloat(uni_name + ".constant", 1.0f);
        lightingShader->setFloat(uni_name + ".linear", 0.09f);
        lightingShader->setFloat(uni_name + ".quadratic", 0.032f);
    }

    lightingShader->setVec3("spotLight.ambient", spotAmbient, spotAmbient, spotAmbient);
    lightingShader->setVec3("spotLight.diffuse", spotDiffuse, spotDiffuse, spotDiffuse);
    lightingShader->setVec3("spotLight.specular", spotSpec, spotSpec, spotSpec);
    lightingShader->setVec3("spotLight.position", camera->position);
    lightingShader->setVec3("spotLight.direction", camera->front);
    lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    lightingShader->setFloat("spotLight.constant", 1.0f);
    lightingShader->setFloat("spotLight.linear", 0.09f);
    lightingShader->setFloat("spotLight.quadratic", 0.032f);

    // Material
    lightingShader->setFloat("material.shininess", 64.0f);

    // Drawing
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat,
                              glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
    modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader->setMat4("model", modelMat);
    model->draw(*lightingShader);

    // Lamp cube
    // lampShader->use();

    // for (int i = 0; i < pointLightPositions.size(); i++) {
    //     modelMat = glm::mat4();
    //     modelMat = glm::translate(modelMat, pointLightPositions[i]);
    //     modelMat = glm::scale(modelMat, glm::vec3(0.2f));
    //     lampShader->setMat4("model", modelMat);
    //     lampShader->setMat4("view", camera->getViewMatrix());
    //     lampShader->setMat4("projection", projection);

    //     glBindVertexArray(lightVAO);
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

int main() {
    glfwInit();
    // OpenGL 版本配置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "3DG Project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 视口设置
    glViewport(0, 0, screenWidth, screenHeight);
    // 注册回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // 捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 绘制准备
    prepareDraw();

    // 绘制循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);

        // 清屏
        glClearColor(1.0f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        drawStaff();

        // 计算帧率
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 交换帧缓冲
        glfwSwapBuffers(window);
        // 处理相关事件
        glfwPollEvents();
    }

    delete lightingShader;
    glfwTerminate();
    return 0;
}

/*
 * 回调
 */

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

bool mouseCap = true;
double lstChangeMouse = 0;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // 移动相机
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->processKeyboard(RIGHT, deltaTime);
    // 释放鼠标
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        double now = glfwGetTime();
        if (now - lstChangeMouse > 0.2) {
            lstChangeMouse = now;
            mouseCap = !mouseCap;
            if (mouseCap) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (mouseCap)
        camera->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera->processMouseScroll(yoffset);
}