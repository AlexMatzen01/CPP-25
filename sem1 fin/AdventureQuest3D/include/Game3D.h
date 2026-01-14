#ifndef GAME3D_H
#define GAME3D_H

#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Renderer.h"
#include <memory>

class Game3D {
public:
    Game3D();
    ~Game3D();

    void run();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Renderer> m_renderer;

    float m_deltaTime;
    float m_lastFrame;
    double m_lastMouseX, m_lastMouseY;
    bool m_firstMouse;
};

#endif // GAME3D_H
