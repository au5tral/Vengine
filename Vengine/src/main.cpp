#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>

// Глобальные переменные
Camera camera;
float lastX = 640, lastY = 360;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main() {
    try {
        // Создание окна
        Window window(1280, 720, "My 3D Engine");

        // Настройка мыши
        glfwSetCursorPosCallback(window.getHandle(), mouseCallback);
        glfwSetScrollCallback(window.getHandle(), scrollCallback);
        glfwSetInputMode(window.getHandle(), GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);

        // Загрузка шейдера
        Shader shader("assets/shaders/basic.vert",
            "assets/shaders/basic.frag");

        // Загрузка модели
        Model model("assets/models/Cube.fbx");
        model.position = glm::vec3(0.0f, 0.0f, 0.0f);
        model.scale = glm::vec3(1.0f);

        // Настройка освещения
        glm::vec3 lightPos(5.0f, 10.0f, 5.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        std::cout << "Engine started successfully!" << std::endl;
        std::cout << "Controls: WASD - move, Mouse - look, Scroll - zoom"
            << std::endl;

        // Главный цикл
        while (!window.shouldClose()) {
            // Delta time
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Ввод
            processInput(window.getHandle());

            // Очистка экрана
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Активация шейдера
            shader.use();

            // Матрицы камеры
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 projection = camera.getProjectionMatrix(
                window.getAspectRatio());

            shader.setMat4("view", view);
            shader.setMat4("projection", projection);

            // Освещение
            shader.setVec3("lightPos", lightPos);
            shader.setVec3("viewPos", camera.position);
            shader.setVec3("lightColor", lightColor);
            shader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
            shader.setBool("useTexture", true);

            // Вращение модели (опционально)
            model.rotation.y += 20.0f * deltaTime;

            // Отрисовка модели
            model.draw(shader);

            // Swap buffers
            window.swapBuffers();
            window.pollEvents();
        }

        model.cleanup();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(3, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(4, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.processKeyboard(5, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouse(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.fov -= (float)yoffset;
    if (camera.fov < 1.0f) camera.fov = 1.0f;
    if (camera.fov > 90.0f) camera.fov = 90.0f;
}