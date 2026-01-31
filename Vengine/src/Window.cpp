#include "Window.h"
#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

// Вспомогательная функция для попытки создания окна с указанной версией OpenGL
static GLFWwindow* tryCreateWindow(int major, int minor, int width, int height,
    const std::string& title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(),
        nullptr, nullptr);
    if (window) {
        std::cout << "Created OpenGL " << major << "." << minor
            << " context" << std::endl;
    }
    return window;
}

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height)
{
    // Инициализация GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Callback для ошибок GLFW
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
        });

    // Пробуем создать окно с разными версиями OpenGL (от новой к старой)
    struct { int major; int minor; } versions[] = {
        {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
        {3, 3}  // Минимум для современного OpenGL
    };

    m_window = nullptr;
    for (const auto& v : versions) {
        m_window = tryCreateWindow(v.major, v.minor, width, height, title);
        if (m_window) break;
    }

    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error(
            "Failed to create GLFW window!\n"
            "Your GPU may not support OpenGL 3.3+\n"
            "Please update your graphics drivers!"
        );
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Включение глубины и MSAA
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Вывод информации о GPU
    std::cout << "================================" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "================================" << std::endl;
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::framebufferSizeCallback(GLFWwindow* window,
    int width, int height) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->m_width = width;
    win->m_height = height;
    glViewport(0, 0, width, height);
}