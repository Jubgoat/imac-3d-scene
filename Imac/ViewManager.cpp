/*
 * ViewManager.cpp
 * Implementation of window creation, camera control, and input handling
 */

#define GLM_ENABLE_EXPERIMENTAL
#include "ViewManager.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

namespace
{
    // Initial window dimensions
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 900;

    // Shader uniform names
    const char* const g_ViewName = "view";
    const char* const g_ProjectionName = "projection";
    const char* const g_ViewPosName = "viewPos";

    // Camera and input tuning values
    const float MOUSE_SENSITIVITY = 0.1f;
    const float MAX_PITCH = 89.0f;
    const float MIN_SPEED = 0.5f;
    const float MAX_SPEED = 10.0f;
    const float SPEED_STEP = 0.5f;
    const float MAX_DELTA_TIME = 0.1f;   // caps the first frame and any long stall
    const float ORTHO_SCALE = 5.0f;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE = 100.0f;

    // Global camera and input state (shared with the static GLFW callbacks)
    Camera* g_pCamera = nullptr;
    int gWindowWidth = WINDOW_WIDTH;
    int gWindowHeight = WINDOW_HEIGHT;
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;
    float gDeltaTime = 0.0f;
    double gLastFrameTime = 0.0;

    // Projection mode flag
    bool bOrthographicProjection = false;

    // Camera orientation and movement speed (speed is adjustable with the scroll wheel)
    float yaw = -90.0f;
    float pitch = -10.0f;
    float cameraSpeed = 2.5f;

    // Rebuilds the camera's front vector from the current yaw and pitch
    void UpdateCameraFront()
    {
        if (g_pCamera == nullptr)
        {
            return;
        }

        glm::vec3 front;
        front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        front.y = std::sin(glm::radians(pitch));
        front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        g_pCamera->Front = glm::normalize(front);
    }

    // Current aspect ratio, guarded against a zero-height (minimized) window
    float GetAspectRatio()
    {
        if (gWindowHeight <= 0 || gWindowWidth <= 0)
        {
            return static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
        }
        return static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
    }
}

/*
 * Constructor - initializes the view manager and camera
 */
ViewManager::ViewManager(ShaderManager* pShaderManager)
    : m_pShaderManager(pShaderManager),
    m_pWindow(nullptr),
    m_pCamera(new Camera())
{
    g_pCamera = m_pCamera;

    // Starting position to view the entire iMac scene
    m_pCamera->Position = glm::vec3(0.0f, 3.0f, 10.0f);
    m_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pCamera->Zoom = 45.0f;

    // Make the front vector match the starting yaw and pitch so the view does not snap
    UpdateCameraFront();
}

/*
 * Destructor - cleanup
 */
ViewManager::~ViewManager()
{
    if (g_pCamera == m_pCamera)
    {
        g_pCamera = nullptr;
    }

    delete m_pCamera;
    m_pCamera = nullptr;
    m_pShaderManager = nullptr;
    m_pWindow = nullptr;
}

/*
 * Creates the display window, makes its OpenGL context current, and sets up callbacks
 */
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, nullptr, nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window (the requested OpenGL version may not be supported)" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Without a current context, glewInit() fails with "Missing GL version"
    glfwMakeContextCurrent(window);

    // Capture the mouse for camera control
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set callback functions
    glfwSetCursorPosCallback(window, ViewManager::Mouse_Position_Callback);
    glfwSetScrollCallback(window, ViewManager::Mouse_Scroll_Callback);
    glfwSetFramebufferSizeCallback(window, ViewManager::Framebuffer_Size_Callback);

    // Match the viewport and aspect ratio to the real framebuffer size
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    Framebuffer_Size_Callback(window, framebufferWidth, framebufferHeight);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Start the frame timer now so the first frame has a sensible delta time
    gLastFrameTime = glfwGetTime();

    m_pWindow = window;
    return window;
}

/*
 * Callback for window resize events
 */
void ViewManager::Framebuffer_Size_Callback(GLFWwindow* window, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, width, height);
}

/*
 * Callback for mouse movement - controls camera orientation
 */
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
    if (g_pCamera == nullptr)
    {
        return;
    }

    // Handle first mouse input
    if (gFirstMouse)
    {
        gLastX = static_cast<float>(xMousePos);
        gLastY = static_cast<float>(yMousePos);
        gFirstMouse = false;
    }

    // Calculate mouse offset (y is reversed: screen y goes top to bottom)
    float xoffset = static_cast<float>(xMousePos) - gLastX;
    float yoffset = gLastY - static_cast<float>(yMousePos);
    gLastX = static_cast<float>(xMousePos);
    gLastY = static_cast<float>(yMousePos);

    // Apply mouse sensitivity and update yaw and pitch
    yaw += xoffset * MOUSE_SENSITIVITY;
    pitch += yoffset * MOUSE_SENSITIVITY;

    // Constrain pitch to prevent the view from flipping
    if (pitch > MAX_PITCH) pitch = MAX_PITCH;
    if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;

    UpdateCameraFront();
}

/*
 * Callback for mouse scroll - adjusts camera movement speed
 */
void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraSpeed += static_cast<float>(yoffset) * SPEED_STEP;

    // Clamp speed to a reasonable range
    if (cameraSpeed < MIN_SPEED) cameraSpeed = MIN_SPEED;
    if (cameraSpeed > MAX_SPEED) cameraSpeed = MAX_SPEED;

    std::cout << "Camera speed: " << cameraSpeed << std::endl;
}

/*
 * Process keyboard input for camera movement and view switching
 */
void ViewManager::ProcessKeyboardEvents()
{
    if (m_pWindow == nullptr || g_pCamera == nullptr)
    {
        return;
    }

    // Close window on ESC
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_pWindow, true);
    }

    // Switch to Perspective projection (P key)
    if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS && bOrthographicProjection)
    {
        bOrthographicProjection = false;
        std::cout << "Switched to Perspective view" << std::endl;
    }

    // Switch to Orthographic projection (O key)
    if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS && !bOrthographicProjection)
    {
        bOrthographicProjection = true;
        std::cout << "Switched to Orthographic view" << std::endl;
    }

    // Camera movement using speed adjusted by the scroll wheel
    float adjustedSpeed = cameraSpeed * gDeltaTime;
    glm::vec3 right = glm::normalize(glm::cross(g_pCamera->Front, g_pCamera->Up));

    // Forward/Backward (W/S)
    if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
        g_pCamera->Position += adjustedSpeed * g_pCamera->Front;
    if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
        g_pCamera->Position -= adjustedSpeed * g_pCamera->Front;

    // Left/Right strafe (A/D)
    if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
        g_pCamera->Position -= right * adjustedSpeed;
    if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
        g_pCamera->Position += right * adjustedSpeed;

    // Up/Down (Q/E)
    if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
        g_pCamera->Position += adjustedSpeed * g_pCamera->Up;
    if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
        g_pCamera->Position -= adjustedSpeed * g_pCamera->Up;
}

/*
 * Prepares the scene view: updates the camera and sends matrices to the shader
 */
void ViewManager::PrepareSceneView()
{
    if (m_pShaderManager == nullptr || m_pCamera == nullptr)
    {
        return;
    }

    // Delta time for frame-independent movement (capped so a long stall cannot cause a jump)
    double currentTime = glfwGetTime();
    gDeltaTime = static_cast<float>(currentTime - gLastFrameTime);
    gLastFrameTime = currentTime;
    if (gDeltaTime > MAX_DELTA_TIME)
    {
        gDeltaTime = MAX_DELTA_TIME;
    }

    ProcessKeyboardEvents();

    // View matrix from the camera
    glm::mat4 view = m_pCamera->GetViewMatrix();

    // Projection matrix based on the current mode and window shape
    float aspectRatio = GetAspectRatio();
    glm::mat4 projection;
    if (bOrthographicProjection)
    {
        projection = glm::ortho(-ORTHO_SCALE * aspectRatio, ORTHO_SCALE * aspectRatio,
            -ORTHO_SCALE, ORTHO_SCALE,
            NEAR_PLANE, FAR_PLANE);
    }
    else
    {
        projection = glm::perspective(glm::radians(m_pCamera->Zoom), aspectRatio,
            NEAR_PLANE, FAR_PLANE);
    }

    // Pass matrices and camera position to the shader
    m_pShaderManager->setMat4Value(g_ViewName, view);
    m_pShaderManager->setMat4Value(g_ProjectionName, projection);
    m_pShaderManager->setVec3Value(g_ViewPosName, m_pCamera->Position);
}