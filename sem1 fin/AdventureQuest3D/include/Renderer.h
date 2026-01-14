#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include <memory>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void renderCube(Shader& shader, const Camera& camera, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f));

private:
    std::unique_ptr<Mesh> m_cubeMesh;
    void initCubeMesh();
};

#endif // RENDERER_H
