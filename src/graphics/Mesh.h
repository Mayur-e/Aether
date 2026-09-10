#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

// -----------------------------------------------------------------------
// Vertex layout
// -----------------------------------------------------------------------
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

// -----------------------------------------------------------------------
// Mesh — owns a VAO/VBO/EBO and exposes a single draw() call.
//
// Usage:
//   Mesh mesh(vertices, indices);
//   mesh.draw();
// -----------------------------------------------------------------------
class Mesh
{
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;

    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices);
    ~Mesh();

    // No copy — GPU buffers are unique resources
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move is fine
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Draw with whatever shader is currently bound
    void draw() const;

    // ---- Factory helpers for common primitives ----

    // Unit cube centred at origin
    static Mesh makeCube();

    // Flat XZ grid of given half-size and number of cells
    static Mesh makePlane(float size = 10.0f, int divisions = 1);

    // UV sphere
    static Mesh makeSphere(float radius = 1.0f,
                           unsigned int sectors = 36,
                           unsigned int stacks  = 18);

private:
    void setup();
};
