#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <string>

// -----------------------------------------------------------------------
Renderer::Renderer(const char* vertPath, const char* fragPath)
    : shader_(vertPath, fragPath)
{
    glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------
void Renderer::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// -----------------------------------------------------------------------
void Renderer::begin(const glm::mat4& view,
                     const glm::mat4& projection,
                     const glm::vec3& cameraPos)
{
    view_      = view;
    proj_      = projection;
    cameraPos_ = cameraPos;

    shader_.use();
    shader_.setMat4("view",       view_);
    shader_.setMat4("projection", proj_);
    shader_.setVec3("viewPos",    cameraPos_);

    // Upload all point lights (up to 4)
    int numLights = (int)std::min(lights.size(), (size_t)4);
    shader_.setInt("numLights", numLights);

    for (int i = 0; i < numLights; ++i)
    {
        std::string base = "lights[" + std::to_string(i) + "].";
        const PointLight& l = lights[(size_t)i];
        shader_.setVec3(base + "position",  l.position);
        shader_.setVec3(base + "ambient",   l.ambient);
        shader_.setVec3(base + "diffuse",   l.diffuse);
        shader_.setVec3(base + "specular",  l.specular);
        shader_.setFloat(base + "constant",  l.constant);
        shader_.setFloat(base + "linear",    l.linear);
        shader_.setFloat(base + "quadratic", l.quadratic);
    }
}

// -----------------------------------------------------------------------
void Renderer::submit(const DrawCommand& cmd)
{
    if (!cmd.mesh) return;

    // Normal matrix: transpose(inverse(model)) — computed on CPU
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(cmd.modelMatrix)));

    shader_.setMat4("model",        cmd.modelMatrix);
    shader_.setMat3("normalMatrix", normalMatrix);
    shader_.setVec3("objectColor",  cmd.colour);
    shader_.setFloat("shininess",   cmd.shininess);

    cmd.mesh->draw();
}

// -----------------------------------------------------------------------
void Renderer::end()
{
    // Reserved for future post-processing pass
}

// -----------------------------------------------------------------------
unsigned int Renderer::shaderProgramId() const
{
    return shader_.id;
}
