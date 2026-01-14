#include "Game3D.h"
#include <GLFW/glfw3.h>

Game3D::Game3D() 
    : m_deltaTime(0.0f), m_lastFrame(0.0f), m_firstMouse(true) {
    m_window = std::make_unique<Window>(800, 600, "AdventureQuest 3D");
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    m_shader = std::make_unique<Shader>("shaders/vertex.glsl", "shaders/fragment.glsl");
    m_renderer = std::make_unique<Renderer>();

    m_window->setCursorMode(GLFW_CURSOR_DISABLED);
    m_window->getMousePos(m_lastMouseX, m_lastMouseY);
}

Game3D::~Game3D() {}

void Game3D::run() {
    while (!m_window->shouldClose()) {
        float currentFrame = glfwGetTime();
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        processInput(m_deltaTime);
        update(m_deltaTime);
        render();

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}

void Game3D::processInput(float deltaTime) {
    if (m_window->isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window->getGLFWWindow(), true);

    if (m_window->isKeyPressed(GLFW_KEY_W))
        m_camera->processKeyboard(FORWARD, deltaTime);
    if (m_window->isKeyPressed(GLFW_KEY_S))
        m_camera->processKeyboard(BACKWARD, deltaTime);
    if (m_window->isKeyPressed(GLFW_KEY_A))
        m_camera->processKeyboard(LEFT, deltaTime);
    if (m_window->isKeyPressed(GLFW_KEY_D))
        m_camera->processKeyboard(RIGHT, deltaTime);

    double xpos, ypos;
    m_window->getMousePos(xpos, ypos);

    if (m_firstMouse) {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastMouseX;
    float yoffset = m_lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    m_lastMouseX = xpos;
    m_lastMouseY = ypos;

    m_camera->processMouseMovement(xoffset, yoffset);
}

void Game3D::update(float deltaTime) {
    // Update game logic here (none yet)
}

void Game3D::render() {
    m_window->clear(0.1f, 0.1f, 0.1f, 1.0f);
    m_renderer->renderCube(*m_shader, *m_camera, glm::vec3(0.0f, 0.0f, 0.0f));
}
