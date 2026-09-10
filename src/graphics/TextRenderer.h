#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include "Shader.h"

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
    TextRenderer(unsigned int width, unsigned int height);
    ~TextRenderer();
    
    void loadFont(const std::string& fontPath, unsigned int fontSize);
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    void renderTextWorld(const std::string& text, float x, float y, float scale, glm::vec3 color, const glm::mat4& proj, const glm::mat4& view);

private:
    std::map<char, Character> characters;
    Shader* textShader;
    unsigned int VAO, VBO;
    unsigned int screenWidth, screenHeight;
};
