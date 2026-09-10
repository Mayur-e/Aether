#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// -----------------------------------------------------------------------
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);

    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);
    checkErrors(id, true);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

// -----------------------------------------------------------------------
void Shader::use() const
{
    glUseProgram(id);
}

// -----------------------------------------------------------------------
// Uniform helpers
// -----------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setMat3(const std::string& name, const glm::mat3& m) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}

// -----------------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------------
std::string Shader::readFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(std::string("Shader::readFile — cannot open: ") + path);

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& src)
{
    GLuint shader = glCreateShader(type);
    const char* srcPtr = src.c_str();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    glCompileShader(shader);
    checkErrors(shader, false);
    return shader;
}

void Shader::checkErrors(GLuint object, bool isProgram)
{
    GLint  success;
    char   infoLog[1024];

    if (isProgram)
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            std::cerr << "[Shader] Link error:\n" << infoLog << "\n";
        }
    }
    else
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            std::cerr << "[Shader] Compile error:\n" << infoLog << "\n";
        }
    }
}
