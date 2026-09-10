#include "Mesh.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>

// -----------------------------------------------------------------------
// Construction / destruction
// -----------------------------------------------------------------------
Mesh::Mesh(const std::vector<Vertex>& verts,
           const std::vector<unsigned int>& idxs)
    : vertices(verts), indices(idxs), VAO(0), VBO(0), EBO(0)
{
    setup();
}

Mesh::~Mesh()
{
    if (VAO) { glDeleteVertexArrays(1, &VAO); }
    if (VBO) { glDeleteBuffers(1, &VBO); }
    if (EBO) { glDeleteBuffers(1, &EBO); }
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices))
    , indices(std::move(other.indices))
    , VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
{
    other.VAO = other.VBO = other.EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);

        vertices = std::move(other.vertices);
        indices  = std::move(other.indices);
        VAO = other.VAO; VBO = other.VBO; EBO = other.EBO;
        other.VAO = other.VBO = other.EBO = 0;
    }
    return *this;
}

// -----------------------------------------------------------------------
void Mesh::setup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    // position  — location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // normal    — location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // texCoord  — location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// -----------------------------------------------------------------------
void Mesh::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------
// Primitive factories
// -----------------------------------------------------------------------
Mesh Mesh::makeCube()
{
    // Each face: 4 vertices (unique normals), 6 indices
    std::vector<Vertex> v = {
        // +Z  front
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{0,0}}, {{ 0.5f,-0.5f, 0.5f},{0,0,1},{1,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{1,1}}, {{-0.5f, 0.5f, 0.5f},{0,0,1},{0,1}},
        // -Z  back
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}}, {{-0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}},
        {{-0.5f, 0.5f,-0.5f},{0,0,-1},{1,1}}, {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{0,1}},
        // +X  right
        {{ 0.5f,-0.5f, 0.5f},{1,0,0},{0,0}}, {{ 0.5f,-0.5f,-0.5f},{1,0,0},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{1,0,0},{1,1}}, {{ 0.5f, 0.5f, 0.5f},{1,0,0},{0,1}},
        // -X  left
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},{0,0}}, {{-0.5f,-0.5f, 0.5f},{-1,0,0},{1,0}},
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},{1,1}}, {{-0.5f, 0.5f,-0.5f},{-1,0,0},{0,1}},
        // +Y  top
        {{-0.5f, 0.5f, 0.5f},{0,1,0},{0,0}}, {{ 0.5f, 0.5f, 0.5f},{0,1,0},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{0,1,0},{1,1}}, {{-0.5f, 0.5f,-0.5f},{0,1,0},{0,1}},
        // -Y  bottom
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},{0,0}}, {{ 0.5f,-0.5f,-0.5f},{0,-1,0},{1,0}},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},{1,1}}, {{-0.5f,-0.5f, 0.5f},{0,-1,0},{0,1}},
    };

    std::vector<unsigned int> idx;
    for (unsigned int f = 0; f < 6; ++f)
    {
        unsigned int base = f * 4;
        idx.insert(idx.end(), { base, base+1, base+2,
                                 base, base+2, base+3 });
    }

    return Mesh(v, idx);
}

Mesh Mesh::makePlane(float size, int divisions)
{
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    float step = (2.0f * size) / (float)divisions;
    int   n    = divisions + 1;

    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            Vertex vert;
            vert.position = { -size + col * step, 0.0f, -size + row * step };
            vert.normal   = { 0.0f, 1.0f, 0.0f };
            vert.texCoord = { (float)col / divisions, (float)row / divisions };
            verts.push_back(vert);
        }
    }

    for (int row = 0; row < divisions; ++row)
    {
        for (int col = 0; col < divisions; ++col)
        {
            unsigned int tl = (unsigned int)(row * n + col);
            unsigned int tr = tl + 1;
            unsigned int bl = tl + (unsigned int)n;
            unsigned int br = bl + 1;
            idx.insert(idx.end(), { tl, bl, tr, tr, bl, br });
        }
    }

    return Mesh(verts, idx);
}

Mesh Mesh::makeSphere(float radius, unsigned int sectors, unsigned int stacks)
{
    const float PI = (float)M_PI;
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    for (unsigned int i = 0; i <= stacks; ++i)
    {
        float phi  = PI / 2.0f - PI * (float)i / (float)stacks;
        float y    = radius * std::sin(phi);
        float xzr  = radius * std::cos(phi);

        for (unsigned int j = 0; j <= sectors; ++j)
        {
            float theta = 2.0f * PI * (float)j / (float)sectors;
            Vertex v;
            v.position  = { xzr * std::cos(theta), y, xzr * std::sin(theta) };
            v.normal    = glm::normalize(v.position);
            v.texCoord  = { (float)j / sectors, (float)i / stacks };
            verts.push_back(v);
        }
    }

    for (unsigned int i = 0; i < stacks; ++i)
    {
        unsigned int k1 = i * (sectors + 1);
        unsigned int k2 = k1 + sectors + 1;

        for (unsigned int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)         idx.insert(idx.end(), {k1, k2, k1+1});
            if (i != stacks-1)  idx.insert(idx.end(), {k1+1, k2, k2+1});
        }
    }

    return Mesh(verts, idx);
}
