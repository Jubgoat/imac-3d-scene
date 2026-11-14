/*
 * SceneManager.h
 * Manages 3D scene geometry, texturing, rendering, and transformations
 *
 * Objects in scene: iMac monitor + stand, desk surface, book stack,
 *                   coffee mug, keyboard, mouse, pencil holder w/ pencils
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

class SceneManager
{
public:
    SceneManager(ShaderManager* pShaderManager);
    ~SceneManager();

    // Scene management
    void PrepareScene();
    void RenderScene();

    // Transformation methods
    void SetTransformations(
        glm::vec3 scaleXYZ,
        float XrotationDegrees,
        float YrotationDegrees,
        float ZrotationDegrees,
        glm::vec3 positionXYZ);

    // Texture methods
    void SetTextureUVScale(float uScale, float vScale);
    void BindTexture(GLuint textureID);
    void EnableTexture(bool enable);
    void SetTextureMix(float mix);
    void BindSecondTexture(GLuint textureID);

    // Color setting
    void SetShaderColor(
        float redColorValue,
        float greenColorValue,
        float blueColorValue,
        float alphaValue);

    // Lighting methods
    void SetupSceneLights();
    void SetLightPosition(glm::vec3 lightPos);  // legacy, kept for compatibility
    void SetViewPosition(glm::vec3 viewPos);

    // Initialization
    void Init();

private:
    ShaderManager* m_pShaderManager;
    ShapeMeshes* m_basicMeshes;

    // ----------------------------------------------------------------
    // Interleaved vertex data: Position(3) + Normal(3) + TexCoord(2)
    // ----------------------------------------------------------------
    std::vector<float> m_boxVertices;
    std::vector<float> m_cylinderVertices;
    std::vector<float> m_planeVertices;
    std::vector<float> m_coneVertices;   // used for pencil tips

    // OpenGL buffer objects
    GLuint m_boxVAO, m_boxVBO;
    GLuint m_cylinderVAO, m_cylinderVBO;
    GLuint m_planeVAO, m_planeVBO;
    GLuint m_coneVAO, m_coneVBO;

    // ----------------------------------------------------------------
    // Texture IDs
    // ----------------------------------------------------------------
    GLuint m_metalTextureID;          // grey noise metal  (stand base, connector)
    GLuint m_screenTextureID;         // gradient screen   (monitor screen base)
    GLuint m_woodTextureID;           // wood grain        (desk plane)
    GLuint m_brushedMetalTextureID;   // brushed metal     (monitor body, stand arm, bezel)
    GLuint m_glossyTextureID;         // radial gloss      (screen overlay)
    GLuint m_darkCeramicTextureID;    // dark speckled     (coffee mug)    [TEXTURED OBJECT 1]
    GLuint m_bookCoverTextureID;      // warm red/brown    (book stack)    [TEXTURED OBJECT 2]
    GLuint m_whiteTextureID;          // clean white       (keyboard, pencil holder, mouse)

    // UV scale for texture tiling
    float m_uvScaleU;
    float m_uvScaleV;

    // ----------------------------------------------------------------
    // Geometry generation
    // ----------------------------------------------------------------
    void GenerateBoxGeometry();
    void GenerateCylinderGeometry(int segments = 30);
    void GeneratePlaneGeometry();
    void GenerateConeGeometry(int segments = 24);

    // Buffer setup
    void SetupBoxBuffers();
    void SetupCylinderBuffers();
    void SetupPlaneBuffers();
    void SetupConeBuffers();

    // Procedural texture creation
    void CreateProceduralTextures();

    // ----------------------------------------------------------------
    // Sub-scene render helpers (keep RenderScene readable)
    // ----------------------------------------------------------------
    void RenderIMac();          // monitor panel, screen, stand arm, connector, bezel
    void RenderStandBase();     // cylinder base (separate so it can share cylinder VAO cleanly)
    void RenderDesk();          // large wood-textured plane
    void RenderBookStack();     // 4 stacked boxes with book-cover texture
    void RenderMug();           // dark ceramic cylinder mug
    void RenderKeyboard();      // flat white box
    void RenderMouse();         // small flattened white box
    void RenderPencilHolder();  // white cylinder body + 5 cone pencil tips (composite object)
};