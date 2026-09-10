#pragma once

#include <string>
#include <glad/gl.h>

// -----------------------------------------------------------------------
// Texture2D — loads an image from disk using stb_image and uploads it
// to a GL texture object.
//
// Usage:
//   Texture2D tex("assets/wood.png");
//   tex.bind(0);   // binds to GL_TEXTURE0
//   shader.setInt("uTexture", 0);
// -----------------------------------------------------------------------
class Texture2D
{
public:
    GLuint id     = 0;
    int    width  = 0;
    int    height = 0;
    int    channels = 0;

    // Load from file.  Throws std::runtime_error on failure.
    explicit Texture2D(const std::string& path,
                       bool flipVertically = true);

    // Create a 1×1 solid-colour texture (r,g,b,a in 0-255)
    static Texture2D makeColour(unsigned char r, unsigned char g,
                                unsigned char b, unsigned char a = 255);

    ~Texture2D();

    // No copy
    Texture2D(const Texture2D&)            = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    // Move
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    // Bind to a texture unit (0-based)
    void bind(unsigned int unit = 0) const;

    void unbind() const;

private:
    Texture2D() = default;
    void upload(unsigned char* data);
};
