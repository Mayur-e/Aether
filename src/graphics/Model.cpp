#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include <stb_image.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// -----------------------------------------------------------------------
// ModelMesh
// -----------------------------------------------------------------------
void ModelMesh::draw() const {
    if (diffuseTex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
    }
    if (emissiveTex) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, emissiveTex);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void ModelMesh::free() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (diffuseTex)  glDeleteTextures(1, &diffuseTex);
    if (emissiveTex) glDeleteTextures(1, &emissiveTex);
    VAO = VBO = EBO = diffuseTex = emissiveTex = 0;
}

// -----------------------------------------------------------------------
// Model
// -----------------------------------------------------------------------
bool Model::load(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate       |
        aiProcess_GenSmoothNormals  |
        aiProcess_FlipUVs           |
        aiProcess_CalcTangentSpace  |
        aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[Model] Assimp error: " << importer.GetErrorString() << "\n";
        return false;
    }

    directory_ = fs::path(path).parent_path().string();
    processNode(scene->mRootNode, scene);
    std::cout << "[Model] Loaded '" << path << "' — "
              << meshes_.size() << " mesh(es)\n";
    return true;
}

// -----------------------------------------------------------------------
void Model::processNode(const aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        meshes_.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

// -----------------------------------------------------------------------
ModelMesh Model::processMesh(const aiMesh* mesh, const aiScene* scene) {
    struct Vertex { float x,y,z, nx,ny,nz, u,v; };
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idxs;

    verts.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        bMin_.x = std::min(bMin_.x, v.x);
        bMin_.y = std::min(bMin_.y, v.y);
        bMin_.z = std::min(bMin_.z, v.z);
        bMax_.x = std::max(bMax_.x, v.x);
        bMax_.y = std::max(bMax_.y, v.y);
        bMax_.z = std::max(bMax_.z, v.z);

        if (mesh->HasNormals()) {
            v.nx = mesh->mNormals[i].x;
            v.ny = mesh->mNormals[i].y;
            v.nz = mesh->mNormals[i].z;
        } else { v.nx=0; v.ny=1; v.nz=0; }

        if (mesh->mTextureCoords[0]) {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        } else { v.u=0; v.v=0; }

        verts.push_back(v);
    }

    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
        const aiFace& face = mesh->mFaces[f];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            idxs.push_back(face.mIndices[j]);
    }

    // Material colours & textures
    ModelMesh mm;
    mm.indexCount = (unsigned int)idxs.size();

    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        // Base colour
        aiColor4D diffuse(1,1,1,1);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
            mm.baseColor = {diffuse.r, diffuse.g, diffuse.b};

        // Emissive colour
        aiColor4D emissive(0,0,0,0);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
            mm.emissiveColor = {emissive.r, emissive.g, emissive.b};

        // Diffuse texture
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString texPath;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
            // Embedded texture
            const aiTexture* embedded = scene->GetEmbeddedTexture(texPath.C_Str());
            if (embedded) {
                // Load from memory
                int w,h,ch;
                unsigned char* data = nullptr;
                if (embedded->mHeight == 0) {
                    // Compressed
                    data = stbi_load_from_memory(
                        (unsigned char*)embedded->pcData, embedded->mWidth, &w,&h,&ch,4);
                } else {
                    w=embedded->mWidth; h=embedded->mHeight; ch=4;
                    data = (unsigned char*)embedded->pcData;
                }
                if (data) {
                    glGenTextures(1, &mm.diffuseTex);
                    glBindTexture(GL_TEXTURE_2D, mm.diffuseTex);
                    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                    if (embedded->mHeight == 0 && data) stbi_image_free(data);
                }
            } else {
                // External file
                std::string fullPath = directory_ + "/" + texPath.C_Str();
                mm.diffuseTex = loadTexture(fullPath);
            }
        }
    }

    // Upload to GPU
    glGenVertexArrays(1, &mm.VAO);
    glGenBuffers(1, &mm.VBO);
    glGenBuffers(1, &mm.EBO);

    glBindVertexArray(mm.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mm.VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mm.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size()*sizeof(unsigned int), idxs.data(), GL_STATIC_DRAW);

    constexpr int S = sizeof(Vertex);
    // position loc=0
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,S,(void*)0);
    glEnableVertexAttribArray(0);
    // normal loc=1
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,S,(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // texCoord loc=2
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,S,(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return mm;
}

// -----------------------------------------------------------------------
unsigned int Model::loadTexture(const std::string& path) {
    int w,h,ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) {
        std::cerr << "[Model] Failed to load texture: " << path << "\n";
        return 0;
    }
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

// -----------------------------------------------------------------------
void Model::draw(unsigned int shaderProgram, const glm::mat4& modelMat) const {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(modelMat));

    for (const auto& m : meshes_) {
        glm::vec3 col = (m.diffuseTex == 0) ? m.baseColor : glm::vec3(1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram,"objectColor"),1,glm::value_ptr(col));

        // Pass emissive if uniform exists
        int eLoc = glGetUniformLocation(shaderProgram,"emissiveColor");
        if (eLoc >= 0)
            glUniform3fv(eLoc,1,glm::value_ptr(m.emissiveColor));

        m.draw();
    }
}

// -----------------------------------------------------------------------
void Model::free() {
    for (auto& m : meshes_) m.free();
    meshes_.clear();
}
