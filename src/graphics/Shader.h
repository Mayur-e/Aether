#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

// -----------------------------------------------------------------------
// Shader — compiles a vertex + fragment shader pair and exposes
// typed uniform setters.
// -----------------------------------------------------------------------
class Shader
{
public:
    GLuint id;

    // Build from source file paths
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    // Activate program
    void use() const;

    // Uniform setters
    void setBool (const std::string& name, bool        value) const;
    void setInt  (const std::string& name, int         value) const;
    void setFloat(const std::string& name, float       value) const;
    void setVec2 (const std::string& name, const glm::vec2& v) const;
    void setVec3 (const std::string& name, const glm::vec3& v) const;
    void setVec4 (const std::string& name, const glm::vec4& v) const;
    void setMat3 (const std::string& name, const glm::mat3& m) const;
    void setMat4 (const std::string& name, const glm::mat4& m) const;

private:
    static std::string  readFile(const char* path);
    static GLuint       compileShader(GLenum type, const std::string& src);
    static void         checkErrors(GLuint object, bool isProgram);
};
