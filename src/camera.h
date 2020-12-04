#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/**
 * 相机类
 */
class Camera {
public:
    // 相机默认参数
    constexpr static const float YAW = -90.0f;
    constexpr static const float PITCH = 0.0f;
    constexpr static const float SPEED = 2.5f;
    constexpr static const float SENSITIVITY = 0.05f;
    constexpr static const float ZOOM = 45.0f;

    // 相机参数
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // 欧拉角
    float yaw;
    float pitch;
    // 相机选项
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    /**
     * 向量初始化
     */
    explicit Camera(glm::vec3 pPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 pUp = glm::vec3(0.0f, 1.0f, 0.0f),
                    float pYaw = YAW, float pPitch = PITCH);

    /**
     * 参数初始化
     */
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float pYaw, float pPitch);

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    /**
     * 键盘操作
     */
    void processKeyboard(CameraMovement direction, float deltaTime);

    /**
     * 鼠标操作
     */
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    /**
     * 鼠标滚轮
     */
    void processMouseScroll(float yoffset);

private:
    /**
     * 计算相机向量
     */
    void updateCameraVectors();
};

#endif