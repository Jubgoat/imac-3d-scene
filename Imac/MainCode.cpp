/*
 * CS330 Final Project - Textured 3D iMac Scene
 * Author: Zachary Crostreet
 *
 * This program creates a textured 3D scene of an iMac computer with:
 * - Procedural texture generation (no external image files needed)
 * - Texture tiling and overlapping techniques
 * - Phong lighting model with TWO light sources
 * - Full camera navigation
 * - Perspective/Orthographic projection switching
 */

#define GLM_ENABLE_EXPERIMENTAL

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

namespace
{
    const char* const WINDOW_TITLE = "Textured iMac 3D Model - Zachary Crostreet";
    GLFWwindow* g_Window = nullptr;
    SceneManager* g_SceneManager = nullptr;
    ShaderManager* g_ShaderManager = nullptr;
    ViewManager* g_ViewManager = nullptr;
}

// Function prototypes
bool InitializeGLFW();
bool InitializeGLEW();

/*
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Initialize GLFW
    if (InitializeGLFW() == false)
    {
        return(EXIT_FAILURE);
    }

    // Create shader and view managers
    g_ShaderManager = new ShaderManager();
    g_ViewManager = new ViewManager(g_ShaderManager);

    // Create the window (this also makes the OpenGL context current)
    g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);
    if (g_Window == nullptr)
    {
        return(EXIT_FAILURE);
    }

    // Initialize GLEW (requires the context created above)
    if (InitializeGLEW() == false)
    {
        glfwTerminate();
        return(EXIT_FAILURE);
    }

    // ========================================================================
    // VERTEX SHADER - Passes position, normal, and texture coordinates to fragment shader
    // ========================================================================
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Surface normal for lighting
layout (location = 2) in vec2 aTexCoord;  // Texture coordinates

// Output to fragment shader
out vec3 FragPos;      // Fragment position in world space
out vec3 Normal;       // Surface normal in world space
out vec2 TexCoord;     // Texture coordinates

// Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normals properly (inverse transpose for non-uniform scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoord;
    
    // Final vertex position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

    // ========================================================================
    // FRAGMENT SHADER - TWO-LIGHT Phong lighting with texture support
    // ========================================================================
    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

// Input from vertex shader
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

// Material color
uniform vec3 objectColor;

// Camera position for specular highlights
uniform vec3 viewPos;

// Light source 1 - Main key light (warm white)
uniform vec3 lightPos1;
uniform vec3 lightColor1;

// Light source 2 - Fill light (cool blue)
uniform vec3 lightPos2;
uniform vec3 lightColor2;

// Texture uniforms
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform bool useTexture;
uniform bool useSecondTexture;
uniform float textureMix;

/*
 * Calculate Phong lighting for a single light source
 * Returns the combined ambient + diffuse + specular contribution
 */
vec3 CalculateLight(vec3 lightPos, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    // ===== AMBIENT LIGHTING =====
    // Provides base illumination even in shadows
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor * baseColor;
    
    // ===== DIFFUSE LIGHTING =====
    // Main surface illumination based on angle to light
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;
    
    // ===== SPECULAR LIGHTING =====
    // Shiny highlights where light reflects toward camera
    float specularStrength = 0.6;  // Increased for more visible highlights
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  // 32 = shininess
    vec3 specular = specularStrength * spec * lightColor;
    
    // Combine all lighting components
    return ambient + diffuse + specular;
}

void main()
{
    // Normalize the surface normal
    vec3 norm = normalize(Normal);
    
    // Calculate view direction for specular highlights
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // ===== DETERMINE BASE COLOR =====
    // Either from texture(s) or solid color
    vec3 baseColor;
    if (useTexture) {
        vec4 texColor1 = texture(texture1, TexCoord);
        
        // If using second texture, mix them together
        if (useSecondTexture) {
            vec4 texColor2 = texture(texture2, TexCoord);
            baseColor = mix(texColor1, texColor2, textureMix).rgb;
        } else {
            baseColor = texColor1.rgb;
        }
    } else {
        // Use solid color if no texture
        baseColor = objectColor;
    }
    
    // ===== CALCULATE LIGHTING FROM BOTH SOURCES =====
    // Light 1: Main key light (warm white from upper left)
    vec3 lighting1 = CalculateLight(lightPos1, lightColor1, norm, FragPos, viewDir, baseColor);
    
    // Light 2: Fill light (cool blue from upper right)
    vec3 lighting2 = CalculateLight(lightPos2, lightColor2, norm, FragPos, viewDir, baseColor);
    
    // Combine both lights for final color
    vec3 result = lighting1 + lighting2;
    
    // Output final color (clamp to prevent over-bright areas)
    FragColor = vec4(min(result, vec3(1.0)), 1.0);
}
)";

    // ========================================================================
    // Save shaders to temporary files for loading
    // ========================================================================
    FILE* vertexFile = nullptr;
    FILE* fragmentFile = nullptr;

    // Use fopen_s on Windows, regular fopen on others
#ifdef _WIN32
    fopen_s(&vertexFile, "vertexShader.glsl", "w");
    fopen_s(&fragmentFile, "fragmentShader.glsl", "w");
#else
    vertexFile = fopen("vertexShader.glsl", "w");
    fragmentFile = fopen("fragmentShader.glsl", "w");
#endif

    if (vertexFile && fragmentFile) {
        fwrite(vertexShaderSource, sizeof(char), strlen(vertexShaderSource), vertexFile);
        fwrite(fragmentShaderSource, sizeof(char), strlen(fragmentShaderSource), fragmentFile);
        fclose(vertexFile);
        fclose(fragmentFile);
        std::cout << "Shader files created successfully" << std::endl;
    }
    else {
        std::cout << "ERROR: Failed to create shader files" << std::endl;
        if (vertexFile) fclose(vertexFile);
        if (fragmentFile) fclose(fragmentFile);
        glfwTerminate();
        return(EXIT_FAILURE);
    }

    // Load and activate shaders
    g_ShaderManager->LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    g_ShaderManager->use();

    // Create scene manager and prepare the 3D scene
    g_SceneManager = new SceneManager(g_ShaderManager);
    g_SceneManager->PrepareScene();

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    std::cout << "\n========================================" << std::endl;
    std::cout << "LIGHTING SETUP:" << std::endl;
    std::cout << "  Light 1: Warm white key light (upper left)" << std::endl;
    std::cout << "  Light 2: Cool blue fill light (upper right)" << std::endl;
    std::cout << "  Model: Phong lighting with ambient, diffuse, specular" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ========================================================================
    // MAIN RENDER LOOP
    // ========================================================================
    while (!glfwWindowShouldClose(g_Window))
    {
        // Clear color and depth buffers
        glClearColor(0.95f, 0.95f, 0.97f, 1.0f);  // Light grey background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Prepare view (handles camera updates)
        g_ViewManager->PrepareSceneView();

        // Render the 3D scene (with lighting)
        g_SceneManager->RenderScene();

        // Swap buffers and poll events
        glfwSwapBuffers(g_Window);
        glfwPollEvents();
    }

    // ========================================================================
    // CLEANUP - Release memory and resources
    // ========================================================================
    if (NULL != g_SceneManager) {
        delete g_SceneManager;
        g_SceneManager = NULL;
    }
    if (NULL != g_ViewManager) {
        delete g_ViewManager;
        g_ViewManager = NULL;
    }
    if (NULL != g_ShaderManager) {
        delete g_ShaderManager;
        g_ShaderManager = NULL;
    }

    // Clean up temporary shader files
    remove("vertexShader.glsl");
    remove("fragmentShader.glsl");

    glfwTerminate();
    return(EXIT_SUCCESS);
}

/*
 * Initialize GLFW library and set OpenGL version hints
 */
bool InitializeGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // The shaders are #version 330 core, so request OpenGL 3.3 core.
    // Asking for a newer version than the GPU or driver supports makes window creation fail.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

/*
 * Initialize GLEW library for OpenGL extension loading
 * Must be called after a window has been created and its context made current
 */
bool InitializeGLEW()
{
    glewExperimental = GL_TRUE;   // needed for core profile contexts with older GLEW versions

    GLenum GLEWInitResult = glewInit();
    if (GLEW_OK != GLEWInitResult)
    {
        std::cerr << glewGetErrorString(GLEWInitResult) << std::endl;
        return false;
    }

    // Display OpenGL version and controls
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << "\n" << std::endl;
    std::cout << "===== TEXTURED 3D SCENE WITH DUAL LIGHTING =====" << std::endl;
    std::cout << "WASD     - Move camera forward/left/backward/right" << std::endl;
    std::cout << "QE       - Move camera up/down" << std::endl;
    std::cout << "Mouse    - Look around" << std::endl;
    std::cout << "Scroll   - Adjust movement speed" << std::endl;
    std::cout << "P        - Switch to Perspective view" << std::endl;
    std::cout << "O        - Switch to Orthographic view" << std::endl;
    std::cout << "ESC      - Exit program" << std::endl;
    std::cout << "================================================" << std::endl;

    return true;
}