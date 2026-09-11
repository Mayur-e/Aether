#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

// -----------------------------------------------------------------------
// ModelMesh — a single mesh/submesh inside a loaded Model.
// -----------------------------------------------------------------------
struct ModelMesh {
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount  = 0;
    unsigned int diffuseTex  = 0;   // 0 = use material colour
    unsigned int emissiveTex = 0;
    glm::vec3    baseColor   = {1.0f, 1.0f, 1.0f};
    glm::vec3    emissiveColor = {0.0f, 0.0f, 0.0f};

    void draw() const;
    void free();
};

// -----------------------------------------------------------------------
// Model — loads a GLTF/GLB/FBX/OBJ via Assimp.
// Usage:
//   Model m;
//   m.load("assets/characters/kael/SK_Kael.glb");
//   m.draw(shader, modelMatrix);
// -----------------------------------------------------------------------
class Model {
public:
    bool loaded() const { return !meshes_.empty(); }

    // Load from file (GLTF, GLB, FBX, OBJ).
    // Returns true on success.
    bool load(const std::string& path);

    // Draw all submeshes. The caller must set MVP uniforms on the shader.
    // Uniform interface expected:
    //   uniform mat4  model;
    //   uniform vec3  objectColor;
    //   uniform vec3  emissiveColor;   (optional)
    void draw(unsigned int shaderProgram, const glm::mat4& modelMat) const;

    // Bounding box (model space)
    glm::vec3 boundsMin() const { return bMin_; }
    glm::vec3 boundsMax() const { return bMax_; }
    // Height of the model
    float height() const { return bMax_.y - bMin_.y; }

    void free();

private:
    std::vector<ModelMesh> meshes_;
    std::string            directory_;
    glm::vec3              bMin_{1e9f}, bMax_{-1e9f};

    void processNode(const struct aiNode* node, const struct aiScene* scene);
    ModelMesh processMesh(const struct aiMesh* mesh, const struct aiScene* scene);
    unsigned int loadTexture(const std::string& path);
};
