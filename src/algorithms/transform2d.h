#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// -----------------------------------------------------------------------
// 2D Transformation matrices (homogeneous 3×3, applied to vec3(x,y,1)).
//
// Compose by multiplication (right-to-left application order):
//   glm::mat3 T = Transform2D::translate(tx, ty);
//   glm::mat3 R = Transform2D::rotate(angleDeg);
//   glm::mat3 S = Transform2D::scale(sx, sy);
//   glm::mat3 M = T * R * S;   // scale → rotate → translate
//
//   glm::vec3 p = M * glm::vec3(x, y, 1.0f);
// -----------------------------------------------------------------------

namespace Transform2D
{
    // Translation matrix
    inline glm::mat3 translate(float tx, float ty)
    {
        return glm::mat3(
            1,  0,  0,
            0,  1,  0,
            tx, ty, 1
        );
        // Note: GLM stores column-major, row vectors on left:
        // result[col][row], so we set [2][0]=tx, [2][1]=ty.
    }

    // Rotation matrix (counter-clockwise, degrees)
    inline glm::mat3 rotate(float angleDeg)
    {
        float r = glm::radians(angleDeg);
        float c = std::cos(r);
        float s = std::sin(r);
        return glm::mat3(
             c,  s, 0,
            -s,  c, 0,
             0,  0, 1
        );
    }

    // Non-uniform scale
    inline glm::mat3 scale(float sx, float sy)
    {
        return glm::mat3(
            sx, 0,  0,
            0,  sy, 0,
            0,  0,  1
        );
    }

    // Shear: xShear shifts x by factor of y, yShear shifts y by factor of x
    inline glm::mat3 shear(float shx, float shy)
    {
        return glm::mat3(
            1,   shy, 0,
            shx, 1,   0,
            0,   0,   1
        );
    }

    // Reflection about the X-axis (y → -y)
    inline glm::mat3 reflectX()
    {
        return glm::mat3(
            1,  0, 0,
            0, -1, 0,
            0,  0, 1
        );
    }

    // Reflection about the Y-axis (x → -x)
    inline glm::mat3 reflectY()
    {
        return glm::mat3(
           -1, 0, 0,
            0, 1, 0,
            0, 0, 1
        );
    }

    // Apply transform M to a 2D point (x, y) and return the result
    inline glm::vec2 apply(const glm::mat3& M, float x, float y)
    {
        glm::vec3 p = M * glm::vec3(x, y, 1.0f);
        return { p.x, p.y };
    }

}  // namespace Transform2D
