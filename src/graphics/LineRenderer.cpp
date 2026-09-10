#include "LineRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

// Simple shader to draw raw pixels
const char* lineVertSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 projection;
out vec3 vertexColor;
void main() {
    gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);
    vertexColor = aColor;
}
)";

const char* lineFragSrc = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vertexColor, 1.0);
}
)";

LineRenderer::LineRenderer(int width, int height) : screenWidth_(width), screenHeight_(height) {
    setupShader();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

LineRenderer::~LineRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void LineRenderer::setupShader() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &lineVertSrc, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &lineFragSrc, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void LineRenderer::updateScreenSize(int width, int height) {
    screenWidth_ = width;
    screenHeight_ = height;
}

void LineRenderer::plot(int x, int y, glm::vec3 color) {
    pointBuffer_.push_back({ glm::vec2((float)x, (float)y), color });
}

// -----------------------------------------------------------------------
// CGG: DDA Algorithm Implementation
// -----------------------------------------------------------------------
void LineRenderer::drawDDALine(int x1, int y1, int x2, int y2, glm::vec3 color) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) {
        plot(x1, y1, color);
        return;
    }

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = (float)x1;
    float y = (float)y1;

    for (int i = 0; i <= steps; i++) {
        plot((int)std::round(x), (int)std::round(y), color);
        x += xInc;
        y += yInc;
    }
}

// -----------------------------------------------------------------------
// CGG: Bresenham's Line Algorithm Implementation
// -----------------------------------------------------------------------
void LineRenderer::drawBresenhamLine(int x1, int y1, int x2, int y2, glm::vec3 color) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        plot(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// -----------------------------------------------------------------------
void LineRenderer::renderAll() {
    if (pointBuffer_.empty()) return;

    glUseProgram(shaderProgram);
    
    // Ortho projection matching pixel coordinates (top-left is 0,0 usually, but let's map bottom-left as 0,0 to match standard OpenGL)
    // Actually, screen Y is usually flipped for input/window coordinates, let's keep bottom-left = 0,0
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth_, 0.0f, (float)screenHeight_);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pointBuffer_.size() * sizeof(PointData), pointBuffer_.data(), GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PointData), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PointData), (void*)offsetof(PointData, color));
    glEnableVertexAttribArray(1);

    // Set point size to make pixels visible
    glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, (GLsizei)pointBuffer_.size());

    glBindVertexArray(0);
    pointBuffer_.clear();
}
