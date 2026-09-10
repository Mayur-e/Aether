#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <stdexcept>
#include <utility>

// -----------------------------------------------------------------------
Texture2D::Texture2D(const std::string& path, bool flipVertically)
{
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
        throw std::runtime_error("Texture2D: failed to load '" + path + "' — " + stbi_failure_reason());

    upload(data);
    stbi_image_free(data);
}

Texture2D Texture2D::makeColour(unsigned char r, unsigned char g,
                                unsigned char b, unsigned char a)
{
    Texture2D t;
    t.width    = 1;
    t.height   = 1;
    t.channels = 4;

    unsigned char pixel[4] = { r, g, b, a };

    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    return t;
}

Texture2D::~Texture2D()
{
    if (id) glDeleteTextures(1, &id);
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : id(other.id), width(other.width), height(other.height), channels(other.channels)
{
    other.id = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this != &other)
    {
        if (id) glDeleteTextures(1, &id);
        id = other.id; width = other.width;
        height = other.height; channels = other.channels;
        other.id = 0;
    }
    return *this;
}

// -----------------------------------------------------------------------
void Texture2D::bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

// -----------------------------------------------------------------------
void Texture2D::upload(unsigned char* data)
{
    GLenum fmt = GL_RGB;
    if      (channels == 1) fmt = GL_RED;
    else if (channels == 4) fmt = GL_RGBA;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, (GLint)fmt,
                 width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}
