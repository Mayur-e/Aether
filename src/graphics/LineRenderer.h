#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

// -----------------------------------------------------------------------
// LineRenderer.h
// Software rasterization of lines using DDA and Bresenham algorithms,
// rendered via OpenGL GL_POINTS 
// -----------------------------------------------------------------------
class LineRenderer {
public:
    LineRenderer(int width, int height);
    ~LineRenderer();

    // Renders a line using the DDA algorithm (screen space pixel plotting)
    void drawDDALine(int x1, int y1, int x2, int y2, glm::vec3 color);

    // Renders a line using Bresenham's algorithm (screen space pixel plotting)
    void drawBresenhamLine(int x1, int y1, int x2, int y2, glm::vec3 color);

    void renderAll(); // Flushes the batched points to the screen

    void updateScreenSize(int width, int height);

private:
    int screenWidth_, screenHeight_;

    unsigned int VAO, VBO;
    unsigned int shaderProgram;

    struct PointData {
        glm::vec2 pos;
        glm::vec3 color;
    };
    std::vector<PointData> pointBuffer_;

    void setupShader();
    void plot(int x, int y, glm::vec3 color);
};
