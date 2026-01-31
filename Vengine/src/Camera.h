#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;
    float speed = 5.0f;
    float sensitivity = 0.1f;
    float fov = 45.0f;

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov),
            aspectRatio, 0.1f, 1000.0f);
    }

    void processKeyboard(int direction, float deltaTime) {
        float velocity = speed * deltaTime;
        if (direction == 0) position += front * velocity;      // W
        if (direction == 1) position -= front * velocity;      // S
        if (direction == 2) position -= glm::normalize(
            glm::cross(front, up)) * velocity;                 // A
        if (direction == 3) position += glm::normalize(
            glm::cross(front, up)) * velocity;                 // D
        if (direction == 4) position += up * velocity;         // Space
        if (direction == 5) position -= up * velocity;         // Ctrl
    }

    void processMouse(float xoffset, float yoffset) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }
};