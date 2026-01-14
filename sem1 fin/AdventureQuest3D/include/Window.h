#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void clear(float r, float g, float b, float a);

    bool isKeyPressed(int key) const;
    void getMousePos(double& x, double& y) const;
    void setCursorMode(int mode);

    GLFWwindow* getGLFWWindow() const { return m_window; }

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
};

#endif // WINDOW_H
