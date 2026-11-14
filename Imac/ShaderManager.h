#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
    void UseProgram(GLuint program);
    void use(); //

    void SetMat4(GLuint program, const std::string& name, const glm::mat4& mat);
    void SetVec3(GLuint program, const std::string& name, const glm::vec3& value);
    void SetFloat(GLuint program, const std::string& name, float value);
    void SetInt(GLuint program, const std::string& name, int value);
    void SetBool(GLuint program, const std::string& name, bool value);

    // Compatibility methods for SceneManager.cpp
    void setIntValue(const std::string& name, int value);
    void setBoolValue(const std::string& name, bool value);
    void setFloatValue(const std::string& name, float value);
    void setVec3Value(const std::string& name, const glm::vec3& value);
    void setMat4Value(const std::string& name, const glm::mat4& value);
    void LoadShaders(const char* vertexPath, const char* fragmentPath);

private:
    GLuint m_CurrentProgram;
};