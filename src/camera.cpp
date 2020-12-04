#include "camera.h"

Camera::Camera(glm::vec3 pPosition, glm::vec3 pUp, float pYaw, float pPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED),
                                                                               mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    position = pPosition;
    worldUp = pUp;
    yaw = pYaw;
    pitch = pPitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float pYaw, float pPitch) : front(
        glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    yaw = pYaw;
    pitch = pPitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, double deltaTime) {
    double velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
}

void Camera::processMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // 最大判断
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // 更新
    updateCameraVectors();
}

void Camera::processMouseScroll(double yoffset) {
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    // 计算正方向
    glm::vec3 tFront;
    tFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    tFront.y = sin(glm::radians(pitch));
    tFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(tFront);
    // 计算右、上
    right = glm::normalize(glm::cross(front,
                                      worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}
