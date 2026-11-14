#include "ShapeMeshes.h"
#include <cmath>

ShapeMeshes::ShapeMeshes()
    : m_PlaneVAO(0), m_PlaneVBO(0), m_PlaneEBO(0),
      m_BoxVAO(0), m_BoxVBO(0), m_BoxEBO(0),
      m_CylinderVAO(0), m_CylinderVBO(0), m_CylinderEBO(0),
      m_ConeVAO(0), m_ConeVBO(0), m_ConeEBO(0)
{
    CreatePlane();
    CreateBox();
    CreateCylinder();
    CreateCone();
}

ShapeMeshes::~ShapeMeshes()
{
    glDeleteVertexArrays(1, &m_PlaneVAO);
    glDeleteBuffers(1, &m_PlaneVBO);
    glDeleteBuffers(1, &m_PlaneEBO);
    glDeleteVertexArrays(1, &m_BoxVAO);
    glDeleteBuffers(1, &m_BoxVBO);
    glDeleteBuffers(1, &m_BoxEBO);
    glDeleteVertexArrays(1, &m_CylinderVAO);
    glDeleteBuffers(1, &m_CylinderVBO);
    glDeleteBuffers(1, &m_CylinderEBO);
    glDeleteVertexArrays(1, &m_ConeVAO);
    glDeleteBuffers(1, &m_ConeVBO);
    glDeleteBuffers(1, &m_ConeEBO);
}

void ShapeMeshes::CreatePlane()
{
    float vertices[] = {
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &m_PlaneVAO);
    glBindVertexArray(m_PlaneVAO);
    glGenBuffers(1, &m_PlaneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_PlaneEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_PlaneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void ShapeMeshes::CreateBox()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 6, 4, 7,
        8, 9, 10, 10, 11, 8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    glGenVertexArrays(1, &m_BoxVAO);
    glBindVertexArray(m_BoxVAO);
    glGenBuffers(1, &m_BoxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_BoxEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BoxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void ShapeMeshes::CreateCylinder()
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int segments = 30;

    float radius = 0.5f;
    float height = 1.0f;

    // Top circle
    for (int i = 0; i < segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((float)i / segments);
        vertices.push_back(1.0f);
    }

    // Bottom circle
    for (int i = 0; i < segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f);
    }

    // Side normals
    for (int i = 0; i < segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float nx = cos(angle);
        float nz = sin(angle);

        // Top vertex
        vertices.push_back(radius * nx);
        vertices.push_back(height / 2.0f);
        vertices.push_back(radius * nz);
        vertices.push_back(nx);
        vertices.push_back(0.0f);
        vertices.push_back(nz);
        vertices.push_back((float)i / segments);
        vertices.push_back(1.0f);

        // Bottom vertex
        vertices.push_back(radius * nx);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(radius * nz);
        vertices.push_back(nx);
        vertices.push_back(0.0f);
        vertices.push_back(nz);
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f);
    }

    glGenVertexArrays(1, &m_CylinderVAO);
    glBindVertexArray(m_CylinderVAO);
    glGenBuffers(1, &m_CylinderVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void ShapeMeshes::CreateCone()
{
    std::vector<float> vertices;
    int segments = 24;

    float radius = 0.5f;
    float height = 1.0f;

    // Apex
    vertices.push_back(0.0f);
    vertices.push_back(height / 2.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.5f);
    vertices.push_back(1.0f);

    // Base circle
    for (int i = 0; i < segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f);
    }

    glGenVertexArrays(1, &m_ConeVAO);
    glBindVertexArray(m_ConeVAO);
    glGenBuffers(1, &m_ConeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_ConeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void ShapeMeshes::RenderPlane(float width, float height, glm::vec3 position, glm::vec3 color)
{
    glBindVertexArray(m_PlaneVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ShapeMeshes::RenderBox(float width, float height, float depth, glm::vec3 position, glm::vec3 color)
{
    glBindVertexArray(m_BoxVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void ShapeMeshes::RenderCylinder(float radius, float height, int segments, glm::vec3 position, glm::vec3 color)
{
    glBindVertexArray(m_CylinderVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, segments * 6);
}

void ShapeMeshes::RenderCone(float radius, float height, int segments, glm::vec3 position, glm::vec3 color)
{
    glBindVertexArray(m_ConeVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
}