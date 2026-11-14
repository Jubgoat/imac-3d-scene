/*
 * SceneManager.cpp
 * Implementation of 3D scene geometry generation, texturing, and rendering
 *
 * Scene replicates a desk photo containing:
 *   - Apple iMac monitor with stand        (box + cylinder primitives)
 *   - Wood desk surface                    (plane primitive)
 *   - Stack of books (left side)           (box primitive)  [TEXTURED]
 *   - Dark coffee mug (left of keyboard)   (cylinder primitive) [TEXTURED]
 *   - Keyboard (front-center)              (box primitive)
 *   - Mouse (right of keyboard)            (box primitive)
 *   - Pencil/pen holder (far right)        (cylinder + cones = composite)
 *
 * Primitives used: Box, Cylinder, Plane, Cone  (4 required types)
 * Textured objects: mug (dark ceramic), book stack (book cover), screen (mixed)
 * Lights: warm-white key light + cool-blue fill light (Phong model)
 *
 * Uses procedural textures - no external image files required.
 */

#define GLM_ENABLE_EXPERIMENTAL
#include "SceneManager.h"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <cmath>

namespace
{
    // Shader uniform names
    const char* g_ModelName = "model";
    const char* g_ColorValueName = "objectColor";
    const char* g_TextureValueName = "texture1";
    const char* g_Texture2ValueName = "texture2";
    const char* g_UseTextureName = "useTexture";
    const char* g_UseSecondTextureName = "useSecondTexture";
    const char* g_TextureMixName = "textureMix";
}

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

/*
 * Constructor - stores the shader manager and sets safe starting values.
 * Geometry and textures are created later, in Init(), once the
 * OpenGL context and shaders are ready.
 */
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();
    m_uvScaleU = 1.0f;
    m_uvScaleV = 1.0f;

    // Start every OpenGL object name at 0 ("no object") so nothing is
    // deleted or bound before Init() has created it
    m_boxVAO = 0;       m_boxVBO = 0;
    m_cylinderVAO = 0;  m_cylinderVBO = 0;
    m_planeVAO = 0;     m_planeVBO = 0;
    m_coneVAO = 0;      m_coneVBO = 0;

    m_metalTextureID = 0;
    m_screenTextureID = 0;
    m_woodTextureID = 0;
    m_brushedMetalTextureID = 0;
    m_glossyTextureID = 0;
    m_darkCeramicTextureID = 0;
    m_bookCoverTextureID = 0;
    m_whiteTextureID = 0;
}

/*
 * Destructor - releases all GPU and heap resources
 */
SceneManager::~SceneManager()
{
    m_pShaderManager = NULL;
    delete m_basicMeshes;
    m_basicMeshes = NULL;

    // Delete VAOs / VBOs
    glDeleteVertexArrays(1, &m_boxVAO);
    glDeleteBuffers(1, &m_boxVBO);
    glDeleteVertexArrays(1, &m_cylinderVAO);
    glDeleteBuffers(1, &m_cylinderVBO);
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_coneVAO);
    glDeleteBuffers(1, &m_coneVBO);

    // Delete textures
    glDeleteTextures(1, &m_metalTextureID);
    glDeleteTextures(1, &m_screenTextureID);
    glDeleteTextures(1, &m_woodTextureID);
    glDeleteTextures(1, &m_brushedMetalTextureID);
    glDeleteTextures(1, &m_glossyTextureID);
    glDeleteTextures(1, &m_darkCeramicTextureID);
    glDeleteTextures(1, &m_bookCoverTextureID);
    glDeleteTextures(1, &m_whiteTextureID);
}

// ============================================================================
// PROCEDURAL TEXTURE CREATION
// ============================================================================

/*
 * CreateProceduralTextures
 * Generates all textures at runtime using mathematical algorithms.
 * No external image files are required.
 */
void SceneManager::CreateProceduralTextures()
{
    std::cout << "Creating procedural textures..." << std::endl;

    // ------------------------------------------------------------------
    // 1. METAL TEXTURE  - uniform grey with subtle random noise
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int i = 0; i < W * H; i++) {
            int noise = (rand() % 30) - 15;
            unsigned char v = (unsigned char)(180 + noise);
            data[i * 3] = v; data[i * 3 + 1] = v; data[i * 3 + 2] = v;
        }
        glGenTextures(1, &m_metalTextureID);
        glBindTexture(GL_TEXTURE_2D, m_metalTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Metal texture" << std::endl;
    }

    // ------------------------------------------------------------------
    // 2. SCREEN TEXTURE  - white-to-light-grey vertical gradient
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                unsigned char v = (unsigned char)(240 - (y / 8));
                data[i * 3] = v; data[i * 3 + 1] = v; data[i * 3 + 2] = v;
            }
        }
        glGenTextures(1, &m_screenTextureID);
        glBindTexture(GL_TEXTURE_2D, m_screenTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Screen texture" << std::endl;
    }

    // ------------------------------------------------------------------
    // 3. WOOD TEXTURE  - brown with sine-wave grain pattern
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                int grain = (int)(sin(x * 0.1) * 20 + sin(y * 0.05) * 10);
                data[i * 3] = (unsigned char)(139 + grain);
                data[i * 3 + 1] = (unsigned char)(90 + grain / 2);
                data[i * 3 + 2] = (unsigned char)(43 + grain / 3);
            }
        }
        glGenTextures(1, &m_woodTextureID);
        glBindTexture(GL_TEXTURE_2D, m_woodTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Wood texture" << std::endl;
    }

    // ------------------------------------------------------------------
    // 4. BRUSHED METAL TEXTURE  - horizontal sine-wave lines
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                int brush = (int)(sin(y * 0.5) * 15);
                unsigned char v = (unsigned char)(160 + brush);
                data[i * 3] = v; data[i * 3 + 1] = v; data[i * 3 + 2] = v;
            }
        }
        glGenTextures(1, &m_brushedMetalTextureID);
        glBindTexture(GL_TEXTURE_2D, m_brushedMetalTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Brushed metal texture" << std::endl;
    }

    // ------------------------------------------------------------------
    // 5. GLOSSY TEXTURE  - radial gradient (bright centre to dark edge)
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                int dist = (x - 128) * (x - 128) + (y - 128) * (y - 128);
                unsigned char v = (unsigned char)(255 - (dist / 200));
                data[i * 3] = v; data[i * 3 + 1] = v; data[i * 3 + 2] = v;
            }
        }
        glGenTextures(1, &m_glossyTextureID);
        glBindTexture(GL_TEXTURE_2D, m_glossyTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Glossy overlay texture" << std::endl;
    }

    // ------------------------------------------------------------------
    // 6. DARK CERAMIC TEXTURE  - dark charcoal with fine speckle noise
    //    Applied to: coffee mug  [REQUIRED TEXTURED OBJECT]
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int i = 0; i < W * H; i++) {
            int speckle = (rand() % 20) - 10;
            unsigned char v = (unsigned char)(55 + speckle);   // dark grey
            data[i * 3] = v;
            data[i * 3 + 1] = v;
            data[i * 3 + 2] = (unsigned char)(v + 5);          // very slight cool tint
        }
        glGenTextures(1, &m_darkCeramicTextureID);
        glBindTexture(GL_TEXTURE_2D, m_darkCeramicTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Dark ceramic texture (mug)" << std::endl;
    }

    // ------------------------------------------------------------------
    // 7. BOOK COVER TEXTURE  - warm red/brown with subtle linen weave
    //    Applied to: book stack  [REQUIRED TEXTURED OBJECT]
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                // Crosshatch pattern to simulate cloth book cover
                int weave = (int)(sin(x * 1.5) * 8 + sin(y * 1.5) * 8);
                data[i * 3] = (unsigned char)(160 + weave);  // red channel
                data[i * 3 + 1] = (unsigned char)(80 + weave / 2); // green
                data[i * 3 + 2] = (unsigned char)(50 + weave / 3); // blue
            }
        }
        glGenTextures(1, &m_bookCoverTextureID);
        glBindTexture(GL_TEXTURE_2D, m_bookCoverTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: Book cover texture (book stack)" << std::endl;
    }

    // ------------------------------------------------------------------
    // 8. WHITE TEXTURE  - near-white with faint noise for plastic feel
    //    Applied to: keyboard, mouse, pencil holder
    // ------------------------------------------------------------------
    {
        const int W = 256, H = 256;
        unsigned char* data = new unsigned char[W * H * 3];
        for (int i = 0; i < W * H; i++) {
            int noise = (rand() % 10) - 5;
            unsigned char v = (unsigned char)(240 + noise);
            data[i * 3] = v; data[i * 3 + 1] = v; data[i * 3 + 2] = v;
        }
        glGenTextures(1, &m_whiteTextureID);
        glBindTexture(GL_TEXTURE_2D, m_whiteTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        std::cout << "  Created: White plastic texture (keyboard/mouse/holder)" << std::endl;
    }

    std::cout << "All procedural textures created successfully!" << std::endl;
}

// ============================================================================
// GEOMETRY GENERATION
// ============================================================================

/*
 * GenerateBoxGeometry
 * Unit box (-0.5 to +0.5) with per-face normals and UV coordinates.
 * Each vertex: pos(3), normal(3), texcoord(2)  = 8 floats
 */
void SceneManager::GenerateBoxGeometry()
{
    float w = 0.5f, h = 0.5f, d = 0.5f;

    // Front face (+Z)
    m_boxVertices.insert(m_boxVertices.end(), {
        -w,-h, d,  0, 0, 1,  0,0,
         w,-h, d,  0, 0, 1,  1,0,
         w, h, d,  0, 0, 1,  1,1,
         w, h, d,  0, 0, 1,  1,1,
        -w, h, d,  0, 0, 1,  0,1,
        -w,-h, d,  0, 0, 1,  0,0,
        });
    // Back face (-Z)
    m_boxVertices.insert(m_boxVertices.end(), {
        -w,-h,-d,  0, 0,-1,  1,0,
        -w, h,-d,  0, 0,-1,  1,1,
         w, h,-d,  0, 0,-1,  0,1,
         w, h,-d,  0, 0,-1,  0,1,
         w,-h,-d,  0, 0,-1,  0,0,
        -w,-h,-d,  0, 0,-1,  1,0,
        });
    // Left face (-X)
    m_boxVertices.insert(m_boxVertices.end(), {
        -w, h, d, -1, 0, 0,  1,1,
        -w, h,-d, -1, 0, 0,  0,1,
        -w,-h,-d, -1, 0, 0,  0,0,
        -w,-h,-d, -1, 0, 0,  0,0,
        -w,-h, d, -1, 0, 0,  1,0,
        -w, h, d, -1, 0, 0,  1,1,
        });
    // Right face (+X)
    m_boxVertices.insert(m_boxVertices.end(), {
         w, h, d,  1, 0, 0,  0,1,
         w,-h,-d,  1, 0, 0,  1,0,
         w, h,-d,  1, 0, 0,  1,1,
         w,-h,-d,  1, 0, 0,  1,0,
         w, h, d,  1, 0, 0,  0,1,
         w,-h, d,  1, 0, 0,  0,0,
        });
    // Top face (+Y)
    m_boxVertices.insert(m_boxVertices.end(), {
        -w, h,-d,  0, 1, 0,  0,1,
        -w, h, d,  0, 1, 0,  0,0,
         w, h, d,  0, 1, 0,  1,0,
         w, h, d,  0, 1, 0,  1,0,
         w, h,-d,  0, 1, 0,  1,1,
        -w, h,-d,  0, 1, 0,  0,1,
        });
    // Bottom face (-Y)
    m_boxVertices.insert(m_boxVertices.end(), {
        -w,-h,-d,  0,-1, 0,  0,0,
         w,-h,-d,  0,-1, 0,  1,0,
         w,-h, d,  0,-1, 0,  1,1,
         w,-h, d,  0,-1, 0,  1,1,
        -w,-h, d,  0,-1, 0,  0,1,
        -w,-h,-d,  0,-1, 0,  0,0,
        });
}

/*
 * GenerateCylinderGeometry
 * Unit cylinder: radius=1, height=1, centred at origin.
 * Segments controls smoothness. Includes top/bottom caps.
 */
void SceneManager::GenerateCylinderGeometry(int segments)
{
    const float PI = 3.14159265359f;
    const float radius = 1.0f;
    const float halfH = 0.5f;

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments * 2.0f * PI;
        float t2 = (float)(i + 1) / segments * 2.0f * PI;

        float x1 = radius * cos(t1), z1 = radius * sin(t1);
        float x2 = radius * cos(t2), z2 = radius * sin(t2);
        float nx1 = cos(t1), nz1 = sin(t1);
        float nx2 = cos(t2), nz2 = sin(t2);
        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // Side quad (two triangles)
        m_cylinderVertices.insert(m_cylinderVertices.end(), {
            x1,-halfH,z1, nx1,0,nz1, u1,0,
            x2,-halfH,z2, nx2,0,nz2, u2,0,
            x1, halfH,z1, nx1,0,nz1, u1,1,
            x2,-halfH,z2, nx2,0,nz2, u2,0,
            x2, halfH,z2, nx2,0,nz2, u2,1,
            x1, halfH,z1, nx1,0,nz1, u1,1,
            });
        // Top cap
        m_cylinderVertices.insert(m_cylinderVertices.end(), {
            0,halfH,0,   0,1,0, 0.5f,0.5f,
            x1,halfH,z1, 0,1,0, 0.5f + 0.5f * cos(t1), 0.5f + 0.5f * sin(t1),
            x2,halfH,z2, 0,1,0, 0.5f + 0.5f * cos(t2), 0.5f + 0.5f * sin(t2),
            });
        // Bottom cap
        m_cylinderVertices.insert(m_cylinderVertices.end(), {
            0,-halfH,0,  0,-1,0, 0.5f,0.5f,
            x2,-halfH,z2,0,-1,0, 0.5f + 0.5f * cos(t2), 0.5f + 0.5f * sin(t2),
            x1,-halfH,z1,0,-1,0, 0.5f + 0.5f * cos(t1), 0.5f + 0.5f * sin(t1),
            });
    }
}

/*
 * GeneratePlaneGeometry
 * Unit horizontal plane (XZ) at y=0, normal pointing up.
 */
void SceneManager::GeneratePlaneGeometry()
{
    float w = 0.5f, d = 0.5f;
    m_planeVertices.insert(m_planeVertices.end(), {
        -w,0,-d, 0,1,0, 0,0,
         w,0,-d, 0,1,0, 1,0,
         w,0, d, 0,1,0, 1,1,
         w,0, d, 0,1,0, 1,1,
        -w,0, d, 0,1,0, 0,1,
        -w,0,-d, 0,1,0, 0,0,
        });
}

/*
 * GenerateConeGeometry
 * Unit cone: base radius=1 at y=-0.5, apex at y=+0.5.
 * Used for pencil tips in the pencil holder composite object.
 */
void SceneManager::GenerateConeGeometry(int segments)
{
    const float PI = 3.14159265359f;
    const float radius = 1.0f;
    const float baseY = -0.5f;
    const float apexY = 0.5f;

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments * 2.0f * PI;
        float t2 = (float)(i + 1) / segments * 2.0f * PI;

        float x1 = radius * cos(t1), z1 = radius * sin(t1);
        float x2 = radius * cos(t2), z2 = radius * sin(t2);
        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // Outward normal for the slanted side (perpendicular to slant)
        float ny = radius;           // simplified approximation
        float nx1 = cos(t1), nz1 = sin(t1);
        float nx2 = cos(t2), nz2 = sin(t2);
        float nl = sqrt(nx1 * nx1 + ny * ny + nz1 * nz1);

        // Side triangle (base edge to apex)
        m_coneVertices.insert(m_coneVertices.end(), {
            x1,baseY,z1,  nx1 / nl, ny / nl, nz1 / nl,  u1, 0.0f,
            x2,baseY,z2,  nx2 / nl, ny / nl, nz2 / nl,  u2, 0.0f,
            0, apexY, 0,  (nx1 + nx2) / (2 * nl), ny / nl, (nz1 + nz2) / (2 * nl),  (u1 + u2) / 2, 1.0f,
            });
        // Base cap (pointing down)
        m_coneVertices.insert(m_coneVertices.end(), {
            0, baseY, 0,  0,-1,0,  0.5f,0.5f,
            x2,baseY,z2,  0,-1,0,  0.5f + 0.5f * cos(t2), 0.5f + 0.5f * sin(t2),
            x1,baseY,z1,  0,-1,0,  0.5f + 0.5f * cos(t1), 0.5f + 0.5f * sin(t1),
            });
    }
}

// ============================================================================
// BUFFER SETUP
// ============================================================================

/*
 * Helper: create VAO/VBO for a given vertex vector with the standard
 * 8-float interleaved layout: pos(3), normal(3), texcoord(2)
 */
static void UploadToGPU(GLuint& vao, GLuint& vbo, const std::vector<float>& verts)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_STATIC_DRAW);
    // Position  (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal    (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoord  (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void SceneManager::SetupBoxBuffers() { UploadToGPU(m_boxVAO, m_boxVBO, m_boxVertices); }
void SceneManager::SetupCylinderBuffers() { UploadToGPU(m_cylinderVAO, m_cylinderVBO, m_cylinderVertices); }
void SceneManager::SetupPlaneBuffers() { UploadToGPU(m_planeVAO, m_planeVBO, m_planeVertices); }
void SceneManager::SetupConeBuffers() { UploadToGPU(m_coneVAO, m_coneVBO, m_coneVertices); }

/*
 * Init
 * Initializes and prepares all resources for the scene:
 * - Generate geometry for all primitives
 * - Upload geometry to GPU buffers
 * - Create all procedural textures
 */
void SceneManager::Init()
{
    // Generate interleaved vertex data for each primitive type
    GenerateBoxGeometry();
    GenerateCylinderGeometry(30);
    GeneratePlaneGeometry();
    GenerateConeGeometry(24);

    // Upload geometry to GPU
    SetupBoxBuffers();
    SetupCylinderBuffers();
    SetupPlaneBuffers();
    SetupConeBuffers();

    // Generate all procedural textures
    CreateProceduralTextures();
}

// ============================================================================
// SHADER UNIFORM HELPERS
// ============================================================================

void SceneManager::SetTextureUVScale(float uScale, float vScale)
{
    m_uvScaleU = uScale;
    m_uvScaleV = vScale;
}

void SceneManager::BindTexture(GLuint textureID)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (m_pShaderManager)
        m_pShaderManager->setIntValue(g_TextureValueName, 0);
}

void SceneManager::BindSecondTexture(GLuint textureID)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID);
    if (m_pShaderManager) {
        m_pShaderManager->setIntValue(g_Texture2ValueName, 1);
        m_pShaderManager->setBoolValue(g_UseSecondTextureName, true);
    }
}

void SceneManager::EnableTexture(bool enable)
{
    if (m_pShaderManager) {
        m_pShaderManager->setBoolValue(g_UseTextureName, enable);
        if (!enable)
            m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    }
}

void SceneManager::SetTextureMix(float mix)
{
    if (m_pShaderManager)
        m_pShaderManager->setFloatValue(g_TextureMixName, mix);
}

void SceneManager::SetTransformations(
    glm::vec3 scaleXYZ,
    float XrotationDegrees,
    float YrotationDegrees,
    float ZrotationDegrees,
    glm::vec3 positionXYZ)
{
    glm::mat4 model =
        glm::translate(positionXYZ)
        * glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1, 0, 0))
        * glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0, 1, 0))
        * glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0, 0, 1))
        * glm::scale(scaleXYZ);

    if (m_pShaderManager)
        m_pShaderManager->setMat4Value(g_ModelName, model);
}

void SceneManager::SetShaderColor(float r, float g, float b, float /*a*/)
{
    if (m_pShaderManager)
        m_pShaderManager->setVec3Value(g_ColorValueName, glm::vec3(r, g, b));
}

// ============================================================================
// LIGHTING
// ============================================================================

/*
 * SetupSceneLights
 * Configures two Phong light sources:
 *   Light 1 - warm white key light  (upper-left)
 *   Light 2 - cool blue fill light  (upper-right)
 */
void SceneManager::SetupSceneLights()
{
    if (!m_pShaderManager) return;

    m_pShaderManager->setVec3Value("lightPos1", glm::vec3(-6.0f, 10.0f, 8.0f));
    m_pShaderManager->setVec3Value("lightColor1", glm::vec3(1.0f, 0.95f, 0.85f));  // warm white

    m_pShaderManager->setVec3Value("lightPos2", glm::vec3(7.0f, 8.0f, 6.0f));
    m_pShaderManager->setVec3Value("lightColor2", glm::vec3(0.6f, 0.7f, 1.0f));    // cool blue

    std::cout << "Lighting configured: dual-source Phong active" << std::endl;
}

void SceneManager::SetLightPosition(glm::vec3 lightPos)
{
    if (m_pShaderManager)
        m_pShaderManager->setVec3Value("lightPos", lightPos);
}

void SceneManager::SetViewPosition(glm::vec3 viewPos)
{
    if (m_pShaderManager)
        m_pShaderManager->setVec3Value("viewPos", viewPos);
}

// ============================================================================
// SCENE INIT
// ============================================================================

/*
 * PrepareScene
 * Builds all geometry and textures, then configures the lights.
 * The OpenGL context and shaders must already be active when this runs.
 */
void SceneManager::PrepareScene()
{
    // Generate geometry, upload it to the GPU, and create the textures.
    // Without this call the scene has nothing to draw and renders as a blank background.
    Init();

    SetupSceneLights();
    std::cout << "\n========================================" << std::endl;
    std::cout << "Scene setup complete!" << std::endl;
    std::cout << "Objects: iMac, desk, books, mug, keyboard, mouse, pencil holder" << std::endl;
    std::cout << "Primitives: box, cylinder, plane, cone" << std::endl;
    std::cout << "Phong lighting: 2 light sources" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// ============================================================================
// SUB-SCENE RENDER FUNCTIONS
// ============================================================================

/*
 * RenderDesk
 * Large wood-textured horizontal plane that all objects rest on.
 * UV tiling (4x3) gives the grain more realistic density.
 */
void SceneManager::RenderDesk()
{
    SetTransformations(
        glm::vec3(18.0f, 1.0f, 10.0f),
        0, 0, 0,
        glm::vec3(0.0f, 0.0f, 0.0f));
    EnableTexture(true);
    BindTexture(m_woodTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(4.0f, 3.0f);
    glBindVertexArray(m_planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_planeVertices.size() / 8));
}

/*
 * RenderIMac
 * Draws the monitor panel, screen surface, bottom bezel, stand arm, and
 * stand connector using box primitives with brushed-metal / screen textures.
 */
void SceneManager::RenderIMac()
{
    // --- Monitor body (brushed aluminium frame) ---
    SetTransformations(
        glm::vec3(3.5f, 2.2f, 0.3f),
        -5, 0, 0,
        glm::vec3(0.0f, 2.5f, 0.0f));
    EnableTexture(true);
    BindTexture(m_brushedMetalTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));

    // --- Screen surface (gradient base + 20% glossy overlay) ---
    SetTransformations(
        glm::vec3(3.3f, 2.0f, 0.05f),
        -5, 0, 0,
        glm::vec3(0.0f, 2.5f, 0.16f));
    EnableTexture(true);
    BindTexture(m_screenTextureID);
    BindSecondTexture(m_glossyTextureID);
    SetTextureMix(0.2f);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));

    // --- Bottom chin bezel ---
    SetTransformations(
        glm::vec3(3.5f, 0.3f, 0.35f),
        -5, 0, 0,
        glm::vec3(0.0f, 1.4f, 0.0f));
    EnableTexture(true);
    BindTexture(m_brushedMetalTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));

    // --- Stand vertical arm ---
    SetTransformations(
        glm::vec3(0.3f, 2.3f, 0.25f),
        5, 0, 0,
        glm::vec3(0.0f, 1.25f, -0.05f));
    EnableTexture(true);
    BindTexture(m_brushedMetalTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));

    // --- Stand connector piece ---
    SetTransformations(
        glm::vec3(0.6f, 0.5f, 0.15f),
        -5, 0, 0,
        glm::vec3(0.0f, 2.5f, -0.2f));
    EnableTexture(true);
    BindTexture(m_metalTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));
}

/*
 * RenderStandBase
 * Flat aluminium disc cylinder at the base of the iMac stand.
 */
void SceneManager::RenderStandBase()
{
    SetTransformations(
        glm::vec3(1.2f, 0.1f, 1.2f),
        0, 0, 0,
        glm::vec3(0.0f, 0.05f, 0.0f));
    EnableTexture(true);
    BindTexture(m_metalTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_cylinderVertices.size() / 8));
}

/*
 * RenderBookStack
 * Four stacked books to the left of the iMac.
 * Each book is a thin box; alternating slight Y-rotations mimic a casual stack.
 * Book-cover texture applied to all books.  [REQUIRED TEXTURED OBJECT]
 */
void SceneManager::RenderBookStack()
{
    // Book dimensions: wide, thin, deep
    const float bookW = 1.1f, bookH = 0.18f, bookD = 0.85f;

    // Position of stack: left side of desk, near the mug
    const float stackX = -4.5f, stackZ = 1.0f;

    struct BookInfo { float yOff; float yRot; };
    BookInfo books[4] = {
        { 0.09f,  3.0f },   // bottom book
        { 0.27f, -4.0f },
        { 0.45f,  2.0f },
        { 0.63f, -2.5f },   // top book
    };

    EnableTexture(true);
    BindTexture(m_bookCoverTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(1.0f, 1.0f);

    for (int i = 0; i < 4; i++) {
        SetTransformations(
            glm::vec3(bookW, bookH, bookD),
            0, books[i].yRot, 0,
            glm::vec3(stackX, books[i].yOff, stackZ));
        glBindVertexArray(m_boxVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));
    }
}

/*
 * RenderMug
 * Dark charcoal ceramic coffee mug to the left of the keyboard.
 * Cylinder body + thin cylinder rim.
 * Dark ceramic texture applied.  [REQUIRED TEXTURED OBJECT]
 */
void SceneManager::RenderMug()
{
    const float mugX = -2.0f, mugZ = 1.8f;

    // Mug body
    SetTransformations(
        glm::vec3(0.45f, 0.9f, 0.45f),
        0, 0, 0,
        glm::vec3(mugX, 0.45f, mugZ));
    EnableTexture(true);
    BindTexture(m_darkCeramicTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(1.0f, 1.0f);
    glBindVertexArray(m_cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_cylinderVertices.size() / 8));

    // Mug rim (slightly wider, very thin cylinder on top)
    SetTransformations(
        glm::vec3(0.48f, 0.04f, 0.48f),
        0, 0, 0,
        glm::vec3(mugX, 0.92f, mugZ));
    EnableTexture(true);
    BindTexture(m_darkCeramicTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    glBindVertexArray(m_cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_cylinderVertices.size() / 8));
}

/*
 * RenderKeyboard
 * Thin white rectangular box centred in front of the monitor.
 */
void SceneManager::RenderKeyboard()
{
    SetTransformations(
        glm::vec3(2.8f, 0.07f, 0.85f),
        0, 0, 0,
        glm::vec3(0.0f, 0.035f, 2.2f));
    EnableTexture(true);
    BindTexture(m_whiteTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(1.0f, 1.0f);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));
}

/*
 * RenderMouse
 * Small flattened white box to the right of the keyboard.
 */
void SceneManager::RenderMouse()
{
    SetTransformations(
        glm::vec3(0.5f, 0.06f, 0.32f),
        0, 0, 0,
        glm::vec3(1.9f, 0.03f, 2.1f));
    EnableTexture(true);
    BindTexture(m_whiteTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(1.0f, 1.0f);
    glBindVertexArray(m_boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_boxVertices.size() / 8));
}

/*
 * RenderPencilHolder
 * COMPOSITE OBJECT: white cylinder cup + 5 cone pencil tips sticking out the top.
 * Positioned to the far right of the monitor, matching the reference image.
 *
 * This satisfies the requirement for an object made from two or more
 * primitive shapes (cylinder + cone).
 */
void SceneManager::RenderPencilHolder()
{
    const float holderX = 3.8f, holderZ = 1.2f;

    // --- Cylinder cup body ---
    SetTransformations(
        glm::vec3(0.4f, 1.0f, 0.4f),
        0, 0, 0,
        glm::vec3(holderX, 0.5f, holderZ));
    EnableTexture(true);
    BindTexture(m_whiteTextureID);
    m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
    SetTextureUVScale(1.0f, 1.0f);
    glBindVertexArray(m_cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_cylinderVertices.size() / 8));

    // --- Pencil tips (cones) poking out the top ---
    // Five pencils at slightly different offsets and Y-rotations for realism
    struct PencilInfo { float dx; float dz; float yRot; };
    PencilInfo pencils[5] = {
        {  0.0f,  0.0f,   0 },
        {  0.1f,  0.05f, 20 },
        { -0.1f,  0.05f, -15 },
        {  0.05f,-0.1f,  10 },
        { -0.05f,-0.1f, -20 },
    };

    for (int i = 0; i < 5; i++) {
        // Dark grey for pencil barrel (reuse dark ceramic texture)
        SetTransformations(
            glm::vec3(0.07f, 0.55f, 0.07f),
            0, pencils[i].yRot, 0,
            glm::vec3(holderX + pencils[i].dx, 1.27f, holderZ + pencils[i].dz));
        EnableTexture(true);
        BindTexture(m_darkCeramicTextureID);
        m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
        glBindVertexArray(m_cylinderVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_cylinderVertices.size() / 8));

        // Pencil tip cone (wood-coloured, at top of each barrel)
        SetTransformations(
            glm::vec3(0.07f, 0.15f, 0.07f),
            0, pencils[i].yRot, 0,
            glm::vec3(holderX + pencils[i].dx, 1.62f, holderZ + pencils[i].dz));
        EnableTexture(true);
        BindTexture(m_woodTextureID);
        m_pShaderManager->setBoolValue(g_UseSecondTextureName, false);
        glBindVertexArray(m_coneVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_coneVertices.size() / 8));
    }
}

// ============================================================================
// MAIN RENDER LOOP
// ============================================================================

/*
 * RenderScene
 * Uploads light uniforms then calls each sub-scene render function.
 * Light uniforms are re-uploaded here each frame so they stay in sync
 * if the view matrix has been updated.
 */
void SceneManager::RenderScene()
{
    // Refresh dual-light uniforms every frame
    m_pShaderManager->setVec3Value("lightPos1", glm::vec3(-6.0f, 10.0f, 8.0f));
    m_pShaderManager->setVec3Value("lightColor1", glm::vec3(1.0f, 0.95f, 0.85f));  // warm white
    m_pShaderManager->setVec3Value("lightPos2", glm::vec3(7.0f, 8.0f, 6.0f));
    m_pShaderManager->setVec3Value("lightColor2", glm::vec3(0.6f, 0.7f, 1.0f));   // cool blue

    // Draw all objects
    RenderDesk();
    RenderIMac();
    RenderStandBase();
    RenderBookStack();   // box   - book cover texture
    RenderMug();         // cylinder - dark ceramic texture
    RenderKeyboard();    // box   - white plastic texture
    RenderMouse();       // box   - white plastic texture
    RenderPencilHolder();// cylinder + cone (composite) - white + dark ceramic + wood textures

    glBindVertexArray(0);
}