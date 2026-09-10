#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// -----------------------------------------------------------------------
// GameObject
// -----------------------------------------------------------------------
glm::mat4 GameObject::modelMatrix() const
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    m = glm::scale(m, scale);
    return m;
}

// -----------------------------------------------------------------------
// Scene
// -----------------------------------------------------------------------
Scene::Scene()
    : cubeMesh_  (Mesh::makeCube())
    , planeMesh_ (Mesh::makePlane(10.0f, 10))
    , sphereMesh_(Mesh::makeSphere(0.5f, 32, 16))
{
    // ---- Floor ----
    {
        GameObject floor;
        floor.mesh     = &planeMesh_;
        floor.position = { 0.0f, 0.0f, 0.0f };
        floor.colour   = { 0.35f, 0.35f, 0.40f };
        floor.shininess = 8.0f;
        objects.push_back(floor);
    }

    // ---- Centre cube ----
    {
        GameObject box;
        box.mesh      = &cubeMesh_;
        box.position  = { 0.0f, 0.5f, 0.0f };
        box.colour    = { 0.20f, 0.55f, 0.90f };
        box.shininess = 64.0f;
        objects.push_back(box);
    }

    // ---- Smaller orbiting sphere ----
    {
        GameObject orb;
        orb.mesh      = &sphereMesh_;
        orb.position  = { 2.5f, 0.5f, 0.0f };
        orb.colour    = { 0.95f, 0.40f, 0.15f };
        orb.shininess = 128.0f;
        objects.push_back(orb);
    }

    // ---- Corner cubes (decorative) ----
    for (int i = 0; i < 4; ++i)
    {
        float angle    = (float)i * 90.0f;
        float rad      = glm::radians(angle);
        float dist     = 5.0f;

        GameObject corner;
        corner.mesh      = &cubeMesh_;
        corner.position  = { std::cos(rad) * dist, 0.5f, std::sin(rad) * dist };
        corner.scale     = { 0.5f, 0.5f, 0.5f };
        corner.colour    = { 0.9f, 0.85f, 0.1f };
        corner.shininess = 32.0f;
        objects.push_back(corner);
    }

    // ---- Point light ----
    {
        PointLight sun;
        sun.position = { 4.0f, 6.0f, 4.0f };
        sun.diffuse  = { 1.0f, 0.95f, 0.8f };
        lights.push_back(sun);
    }

    // ---- Accent fill light ----
    {
        PointLight fill;
        fill.position = { -5.0f, 3.0f, -3.0f };
        fill.diffuse  = { 0.3f, 0.4f, 0.9f };
        fill.ambient  = { 0.02f, 0.02f, 0.05f };
        lights.push_back(fill);
    }
}

// -----------------------------------------------------------------------
void Scene::update(float dt)
{
    totalTime_ += dt;

    // Rotate the centre cube continuously
    if (objects.size() > 1)
        objects[1].rotation.y = totalTime_ * 45.0f;  // 45 deg/sec

    // Orbit the sphere around the origin
    if (objects.size() > 2)
    {
        float angle     = totalTime_ * 0.8f;          // rad/sec
        float orbitDist = 2.5f;
        objects[2].position.x = std::cos(angle) * orbitDist;
        objects[2].position.z = std::sin(angle) * orbitDist;
        objects[2].rotation.y = -glm::degrees(angle);
    }
}

// -----------------------------------------------------------------------
void Scene::render(Renderer& renderer,
                   const glm::mat4& view,
                   const glm::mat4& projection,
                   const glm::vec3& cameraPos)
{
    renderer.lights = lights;
    renderer.begin(view, projection, cameraPos);

    for (const auto& obj : objects)
    {
        DrawCommand cmd;
        cmd.mesh        = obj.mesh;
        cmd.modelMatrix = obj.modelMatrix();
        cmd.colour      = obj.colour;
        cmd.shininess   = obj.shininess;
        renderer.submit(cmd);
    }

    renderer.end();
}
