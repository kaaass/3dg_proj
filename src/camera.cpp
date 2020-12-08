#include <iostream>
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
        glm::vec3(0.0, 0.0, -1.0)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    yaw = pYaw;
    pitch = pPitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    if (mode == MODE_AUTO) {
        return glm::lookAt(position, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});
    }
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    if (mode == MODE_AUTO)
        return;

    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    if (mode == MODE_DRAG) {
        xoffset = -xoffset;
        yoffset = -yoffset;
    } else if (mode == MODE_AUTO) {
        return;
    }

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

void Camera::processMouseScroll(float yoffset) {
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
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

int Camera::changeMode() {
    mode++;
    if (mode > 2)
        mode = 0;
    std::cout << "Change mode: " << mode << std::endl;
    return mode;
}

void Camera::idle(float delta) {
    if (mode == MODE_AUTO) {
        angle += delta * 10;

        position = glm::vec3{cos(angle / 180 * 3.1415) * 10, 3.0, sin(angle / 180 * 3.1415) * 10};
        front = position - glm::vec3{0, 0, 0};
    }
}
