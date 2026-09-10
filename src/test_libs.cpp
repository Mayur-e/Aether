#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

// define stb_image implementation before including it
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

int main() {
    std::cout << "All libraries included successfully!" << std::endl;
    
    // GLM test
    glm::vec3 v(1.0f, 2.0f, 3.0f);
    std::cout << "GLM vector: " << v.x << ", " << v.y << ", " << v.z << std::endl;
    
    // FreeType test
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    } else {
        std::cout << "FreeType initialized successfully" << std::endl;
        FT_Done_FreeType(ft);
    }
    
    // Assimp test
    Assimp::Importer importer;
    std::cout << "Assimp Importer created successfully" << std::endl;
    
    return 0;
}
