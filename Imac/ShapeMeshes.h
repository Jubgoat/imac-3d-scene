#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class ShapeMeshes
{
public:
    ShapeMeshes();
    ~ShapeMeshes();

    void RenderPlane(float width, float height, glm::vec3 position, glm::vec3 color);
    void RenderBox(float width, float height, float depth, glm::vec3 position, glm::vec3 color);
    void RenderCylinder(float radius, float height, int segments, glm::vec3 position, glm::vec3 color);
    void RenderCone(float radius, float height, int segments, glm::vec3 position, glm::vec3 color);

private:
    void CreatePlane();
    void CreateBox();
    void CreateCylinder();
    void CreateCone();

    GLuint m_PlaneVAO, m_PlaneVBO, m_PlaneEBO;
    GLuint m_BoxVAO, m_BoxVBO, m_BoxEBO;
    GLuint m_CylinderVAO, m_CylinderVBO, m_CylinderEBO;
    GLuint m_ConeVAO, m_ConeVBO, m_ConeEBO;
};