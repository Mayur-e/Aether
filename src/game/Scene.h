#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../graphics/Renderer.h"
#include "../graphics/Mesh.h"

// -----------------------------------------------------------------------
// GameObject — a positioned, coloured mesh instance
// -----------------------------------------------------------------------
struct GameObject
{
    glm::vec3 position  = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation  = { 0.0f, 0.0f, 0.0f };  // degrees (pitch, yaw, roll)
    glm::vec3 scale     = { 1.0f, 1.0f, 1.0f };
    glm::vec3 colour    = { 1.0f, 1.0f, 1.0f };
    float     shininess = 32.0f;

    const Mesh* mesh = nullptr;  // non-owning pointer into Scene's mesh pool

    // Build model matrix from position / rotation / scale
    glm::mat4 modelMatrix() const;
};

// -----------------------------------------------------------------------
// Scene — owns meshes and game objects, drives update + render
// -----------------------------------------------------------------------
class Scene
{
public:
    std::vector<GameObject> objects;
    std::vector<PointLight> lights;

    Scene();

    // dt = elapsed seconds since last frame
    void update(float dt);

    // Render all objects via the provided renderer
    void render(Renderer& renderer,
                const glm::mat4& view,
                const glm::mat4& projection,
                const glm::vec3& cameraPos);

    // Access to the mesh pool (Scene owns the meshes)
    const Mesh* cube()   const { return &cubeMesh_;   }
    const Mesh* plane()  const { return &planeMesh_;  }
    const Mesh* sphere() const { return &sphereMesh_; }

private:
    Mesh cubeMesh_;
    Mesh planeMesh_;
    Mesh sphereMesh_;

    float totalTime_ = 0.0f;
};
