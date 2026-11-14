/*
 * ViewManager.h
 * Manages the display window, camera, projection, and user input
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "ShaderManager.h"

 // Simple free-fly camera
class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    float Zoom;   // vertical field of view in degrees

    Camera(glm::vec3 position = glm::vec3(0.0f, 3.0f, 10.0f))
        : Position(position),
        Front(0.0f, -0.2f, -1.0f),
        Up(0.0f, 1.0f, 0.0f),
        Zoom(45.0f)
    {
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
};

class ViewManager
{
public:
    ViewManager(ShaderManager* pShaderManager);
    ~ViewManager();

    // Window and scene setup
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);
    void PrepareSceneView();

private:
    ShaderManager* m_pShaderManager;
    GLFWwindow* m_pWindow;
    Camera* m_pCamera;

    void ProcessKeyboardEvents();

    // Static callback functions for GLFW
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
    static void Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset);
    static void Framebuffer_Size_Callback(GLFWwindow* window, int width, int height);
};