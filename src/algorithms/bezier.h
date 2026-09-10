#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

// -----------------------------------------------------------------------
// Bezier and Hermite curve utilities — header-only.
//
// Bezier:
//   Uses de Casteljau's algorithm for numerically stable evaluation.
//   Supports any number of control points (degree = n-1).
//
// Hermite:
//   Cubic Hermite curve defined by two endpoints and their tangents.
// -----------------------------------------------------------------------

namespace Curves
{
    // ----------------------------------------------------------------
    // De Casteljau: evaluate Bezier curve at parameter t ∈ [0,1]
    // controlPoints — any number of 2D or 3D points (GLM vec2/vec3)
    // ----------------------------------------------------------------
    template<typename Vec>
    inline Vec bezierEval(const std::vector<Vec>& pts, float t)
    {
        std::vector<Vec> work = pts;
        int n = (int)work.size();
        for (int r = 1; r < n; ++r)
            for (int i = 0; i < n - r; ++i)
                work[(size_t)i] = work[(size_t)i] * (1.0f - t) + work[(size_t)(i + 1)] * t;
        return work[0];
    }

    // Generate `numSamples` evenly-spaced points on the Bezier curve
    template<typename Vec>
    inline std::vector<Vec> bezierCurve(const std::vector<Vec>& controlPoints,
                                        int numSamples = 100)
    {
        std::vector<Vec> result;
        result.reserve((size_t)numSamples + 1);
        for (int i = 0; i <= numSamples; ++i)
        {
            float t = (float)i / (float)numSamples;
            result.push_back(bezierEval(controlPoints, t));
        }
        return result;
    }

    // ----------------------------------------------------------------
    // Cubic Hermite spline evaluation at t ∈ [0,1]
    // p0, p1 — endpoints
    // m0, m1 — tangent vectors at p0 and p1
    // ----------------------------------------------------------------
    template<typename Vec>
    inline Vec hermiteEval(const Vec& p0, const Vec& m0,
                           const Vec& p1, const Vec& m1,
                           float t)
    {
        float t2 = t * t;
        float t3 = t2 * t;

        // Hermite basis functions
        float h00 =  2*t3 - 3*t2 + 1;   // p0
        float h10 =    t3 - 2*t2 + t;   // m0
        float h01 = -2*t3 + 3*t2;       // p1
        float h11 =    t3 -   t2;       // m1

        return p0 * h00 + m0 * h10 + p1 * h01 + m1 * h11;
    }

    // Generate `numSamples` points on the cubic Hermite curve
    template<typename Vec>
    inline std::vector<Vec> hermiteCurve(const Vec& p0, const Vec& m0,
                                         const Vec& p1, const Vec& m1,
                                         int numSamples = 100)
    {
        std::vector<Vec> result;
        result.reserve((size_t)numSamples + 1);
        for (int i = 0; i <= numSamples; ++i)
        {
            float t = (float)i / (float)numSamples;
            result.push_back(hermiteEval(p0, m0, p1, m1, t));
        }
        return result;
    }

}  // namespace Curves
