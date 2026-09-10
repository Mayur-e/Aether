#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// -----------------------------------------------------------------------
// 3D Transformation and Projection matrices (4×4 homogeneous).
//
// Compose right-to-left:
//   glm::mat4 M = Transform3D::translate(2,0,0) * Transform3D::rotateY(45);
//   glm::vec4 p = M * glm::vec4(x, y, z, 1.0f);
// -----------------------------------------------------------------------

namespace Transform3D
{
    // ----- Basic affine transforms -----

    inline glm::mat4 translate(float tx, float ty, float tz)
    {
        return glm::translate(glm::mat4(1.0f), { tx, ty, tz });
    }

    inline glm::mat4 scale(float sx, float sy, float sz)
    {
        return glm::scale(glm::mat4(1.0f), { sx, sy, sz });
    }

    inline glm::mat4 rotateX(float angleDeg)
    {
        return glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg),
                           glm::vec3(1, 0, 0));
    }

    inline glm::mat4 rotateY(float angleDeg)
    {
        return glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg),
                           glm::vec3(0, 1, 0));
    }

    inline glm::mat4 rotateZ(float angleDeg)
    {
        return glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg),
                           glm::vec3(0, 0, 1));
    }

    // Rotation about an arbitrary axis (axis need not be normalised)
    inline glm::mat4 rotateAxis(float angleDeg, glm::vec3 axis)
    {
        return glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg),
                           glm::normalize(axis));
    }

    // ----- Projection matrices -----

    // Orthographic (parallel) projection
    // Maps the box [left,right]×[bottom,top]×[near,far] to NDC [-1,1]³
    inline glm::mat4 orthographic(float left,   float right,
                                  float bottom, float top,
                                  float nearZ,  float farZ)
    {
        return glm::ortho(left, right, bottom, top, nearZ, farZ);
    }

    // Perspective projection (fovY in degrees, aspect = width/height)
    inline glm::mat4 perspective(float fovYDeg, float aspect,
                                 float nearZ = 0.1f, float farZ = 100.0f)
    {
        return glm::perspective(glm::radians(fovYDeg), aspect, nearZ, farZ);
    }

    // Cabinet oblique projection (cavalier: scale=1, alpha=45°)
    // Produces a 4×4 shear matrix that simulates oblique parallel projection.
    // alpha — angle of the receding axis (degrees), scale — foreshortening factor
    inline glm::mat4 oblique(float alphaDeg = 45.0f, float scale = 0.5f)
    {
        float a = glm::radians(alphaDeg);
        float l = scale;
        // Standard oblique shear in XY (z component sheared into x,y)
        glm::mat4 M(1.0f);
        M[2][0] = -l * std::cos(a);   // column 2, row 0
        M[2][1] = -l * std::sin(a);   // column 2, row 1
        return M;
    }

    // Isometric projection matrix (equal-angle dimetric: 30° elevation / 45° azimuth)
    inline glm::mat4 isometric()
    {
        return rotateX(35.264f) * rotateY(-45.0f);
    }

    // View matrix: camera at 'eye', looking at 'target', with 'up' direction
    inline glm::mat4 lookAt(glm::vec3 eye, glm::vec3 target,
                             glm::vec3 up = { 0, 1, 0 })
    {
        return glm::lookAt(eye, target, up);
    }

}  // namespace Transform3D
