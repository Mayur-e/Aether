#include "UIRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ---- Shaders (inline) --------------------------------------------------
static const char* UI_VERT = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
uniform mat4 uProj;
out vec4 vColor;
void main() {
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char* UI_FRAG = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

// -----------------------------------------------------------------------
static unsigned int compileShader(unsigned int type, const char* src) {
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

// -----------------------------------------------------------------------
UIRenderer::UIRenderer(int width, int height)
    : screenWidth_(width), screenHeight_(height)
{
    setupGL();
}

UIRenderer::~UIRenderer() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteProgram(shader_);
}

void UIRenderer::setupGL() {
    unsigned int vs = compileShader(GL_VERTEX_SHADER,   UI_VERT);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, UI_FRAG);
    shader_ = glCreateProgram();
    glAttachShader(shader_, vs);
    glAttachShader(shader_, fs);
    glLinkProgram(shader_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    // Pre-allocate space for 2048 verts (341 quads of 6 verts each)
    glBufferData(GL_ARRAY_BUFFER, 2048 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------
void UIRenderer::beginFrame(int width, int height) {
    screenWidth_  = width;
    screenHeight_ = height;
    verts_.clear();
}

// -----------------------------------------------------------------------
void UIRenderer::addQuad(float x, float y, float w, float h, glm::vec4 c) {
    // Two triangles, top-left origin pixel coords
    float x2 = x + w, y2 = y + h;
    float r = c.r, g = c.g, b = c.b, a = c.a;
    // Triangle 1
    verts_.push_back({x,  y,  r, g, b, a});
    verts_.push_back({x2, y,  r, g, b, a});
    verts_.push_back({x2, y2, r, g, b, a});
    // Triangle 2
    verts_.push_back({x,  y,  r, g, b, a});
    verts_.push_back({x2, y2, r, g, b, a});
    verts_.push_back({x,  y2, r, g, b, a});
}

void UIRenderer::drawRect(float x, float y, float w, float h, glm::vec4 color) {
    addQuad(x, y, w, h, color);
}

// -----------------------------------------------------------------------
void UIRenderer::endFrame() {
    if (verts_.empty()) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Ortho: pixel coords, top-left = (0,0)
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth_,
                                (float)screenHeight_, 0.0f);

    glUseProgram(shader_);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "uProj"), 1, GL_FALSE,
                       glm::value_ptr(proj));

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(verts_.size() * sizeof(Vertex)),
                    verts_.data());
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verts_.size());
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    verts_.clear();
}
