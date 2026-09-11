#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

// -----------------------------------------------------------------------
// UIRenderer — renders flat colored quads in screen-space (2D overlay).
// Used for all menu backgrounds, buttons, and HUD panels.
// Coordinate system: pixels, origin = top-left of the window.
// -----------------------------------------------------------------------
class UIRenderer {
public:
    UIRenderer(int width, int height);
    ~UIRenderer();

    // Call at the start of every UI draw call batch
    void beginFrame(int width, int height);

    // Queue a filled rectangle [x,y] = top-left corner, w/h in pixels
    void drawRect(float x, float y, float w, float h, glm::vec4 color);

    // Flush all queued quads to the screen
    void endFrame();

private:
    int screenWidth_, screenHeight_;

    unsigned int VAO_, VBO_;
    unsigned int shader_;

    struct Vertex {
        float x, y;
        float r, g, b, a;
    };
    std::vector<Vertex> verts_;

    void setupGL();
    void addQuad(float x, float y, float w, float h, glm::vec4 c);
};
