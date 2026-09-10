#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Mesh.h"

#include <vector>

// -----------------------------------------------------------------------
// Light descriptor (point light)
// -----------------------------------------------------------------------
struct PointLight
{
    glm::vec3 position  = { 2.0f, 3.0f, 2.0f };
    glm::vec3 ambient   = { 0.1f, 0.1f, 0.1f };
    glm::vec3 diffuse   = { 0.9f, 0.9f, 0.9f };
    glm::vec3 specular  = { 1.0f, 1.0f, 1.0f };

    float constant  = 1.0f;
    float linear    = 0.09f;
    float quadratic = 0.032f;
};

// -----------------------------------------------------------------------
// DrawCommand — submitted by the Scene each frame
// -----------------------------------------------------------------------
struct DrawCommand
{
    const Mesh*   mesh        = nullptr;
    glm::mat4     modelMatrix = glm::mat4(1.0f);
    glm::vec3     colour      = { 1.0f, 1.0f, 1.0f };
    float         shininess   = 32.0f;
};

// -----------------------------------------------------------------------
// Renderer — owns the lit shader, uploads light uniforms, and executes
// draw commands submitted by the Scene.
//
// Usage:
//   Renderer renderer("shaders/lit.vert", "shaders/lit.frag");
//   renderer.lights.push_back(light);
//   renderer.begin(view, proj, cameraPos);
//     renderer.submit(cmd);
//     ...
//   renderer.end();
// -----------------------------------------------------------------------
class Renderer
{
public:
    std::vector<PointLight> lights;

    Renderer(const char* vertPath, const char* fragPath);

    // Call once per frame before submitting draw commands
    void begin(const glm::mat4& view,
               const glm::mat4& projection,
               const glm::vec3& cameraPos);

    // Submit a single draw call
    void submit(const DrawCommand& cmd);

    // Call after all submissions (currently a no-op, reserved for post-fx)
    void end();

    // Convenience: clear the colour + depth buffer
    static void clear(float r = 0.08f, float g = 0.08f, float b = 0.12f, float a = 1.0f);

private:
    Shader      shader_;
    glm::mat4   view_;
    glm::mat4   proj_;
    glm::vec3   cameraPos_;
};
