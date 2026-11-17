#include "ShaderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

ShaderManager::ShaderManager()
    : m_CurrentProgram(0)
{
}

ShaderManager::~ShaderManager()
{
}

GLuint ShaderManager::CreateShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void ShaderManager::UseProgram(GLuint program)
{
    glUseProgram(program);
    m_CurrentProgram = program;
}

void ShaderManager::use()
{
    glUseProgram(m_CurrentProgram);
}

ShaderManager::ShaderManager()
    : m_CurrentProgram(0),
    m_CachedProgram(0)
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

ShaderManager::~ShaderManager()
{
    if (m_CurrentProgram != 0)
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}

void ShaderManager::SetFloat(GLuint program, const std::string& name, float value)
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1f(loc, value);
}

void ShaderManager::SetInt(GLuint program, const std::string& name, int value)
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1i(loc, value);
}

void ShaderManager::SetBool(GLuint program, const std::string& name, bool value)
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1i(loc, value ? 1 : 0);
}

void ShaderManager::setIntValue(const std::string& name, int value)
{
    SetInt(m_CurrentProgram, name, value);
}

void ShaderManager::setBoolValue(const std::string& name, bool value)
{
    SetBool(m_CurrentProgram, name, value);
}

void ShaderManager::setFloatValue(const std::string& name, float value)
{
    SetFloat(m_CurrentProgram, name, value);
}

void ShaderManager::setVec3Value(const std::string& name, const glm::vec3& value)
{
    SetVec3(m_CurrentProgram, name, value);
}

void ShaderManager::setMat4Value(const std::string& name, const glm::mat4& value)
{
    SetMat4(m_CurrentProgram, name, value);
}

void ShaderManager::LoadShaders(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexSource;
    std::string fragmentSource;
    
    if (!ReadTextFile(vertexPath, vertexSource) || !ReadTextFile(fragmentPath, fragmentSource))
    {
        return false;
    }
    
    // Create shader program
    GLuint program = CreateShaderProgram(vertexSource.c_str(), fragmentSource.c_str());
    if (program == 0)
    {
        return false;
    }

    // Replace any program loaded earlier so it is not leaked
    GLuint oldProgram = m_CurrentProgram;
    UseProgram(program);
    if (oldProgram != 0 && oldProgram != program)
    {
        glDeleteProgram(oldProgram);
    }
    return true;
}