#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

// -----------------------------------------------------------------------
// Polygon fill algorithms — header-only.
//
//  1. Scanline polygon fill (edge table / active edge table method)
//  2. Boundary flood fill (4-connected recursive)
// -----------------------------------------------------------------------

namespace FillAlgorithms
{
    // ----------------------------------------------------------------
    // 2D integer point
    // ----------------------------------------------------------------
    struct Point2i { int x, y; };

    // ----------------------------------------------------------------
    // Scanline polygon fill
    //
    // polygon  — closed polygon vertices (last edge implicitly connects
    //            last to first vertex)
    // fillPixel — callback invoked for each interior pixel (x, y)
    //
    // Algorithm:
    //   For each scanline y between yMin and yMax:
    //     1. Find all edges that cross scanline y
    //     2. Compute x-intercepts, sort them
    //     3. Fill between paired intercepts (parity fill rule)
    // ----------------------------------------------------------------
    inline void scanlineFill(const std::vector<Point2i>& polygon,
                             std::function<void(int x, int y)> fillPixel)
    {
        if (polygon.size() < 3) return;

        // Find y-range
        int yMin = polygon[0].y, yMax = polygon[0].y;
        for (const auto& p : polygon)
        {
            yMin = std::min(yMin, p.y);
            yMax = std::max(yMax, p.y);
        }

        int n = (int)polygon.size();

        for (int y = yMin; y <= yMax; ++y)
        {
            std::vector<float> intersections;

            for (int i = 0; i < n; ++i)
            {
                const Point2i& p1 = polygon[(size_t)i];
                const Point2i& p2 = polygon[(size_t)((i + 1) % n)];

                // Edge must span scanline y
                if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y))
                {
                    // x-intercept by linear interpolation
                    float x = p1.x + (float)(y - p1.y) / (float)(p2.y - p1.y)
                                   * (float)(p2.x - p1.x);
                    intersections.push_back(x);
                }
            }

            std::sort(intersections.begin(), intersections.end());

            // Fill between pairs
            for (size_t k = 0; k + 1 < intersections.size(); k += 2)
            {
                int xStart = (int)std::ceil(intersections[k]);
                int xEnd   = (int)std::floor(intersections[k + 1]);
                for (int x = xStart; x <= xEnd; ++x)
                    fillPixel(x, y);
            }
        }
    }

    // ----------------------------------------------------------------
    // Boundary flood fill (4-connected)
    //
    // getPixelColour — returns a comparable colour for pixel (x,y)
    // setPixel       — sets a pixel at (x,y)
    // boundary       — the border colour (stop expanding when reached)
    // fill           — the interior fill colour
    //
    // Uses an iterative queue to avoid stack overflow on large regions.
    // ----------------------------------------------------------------
    template<typename Colour>
    inline void boundaryFill4(int seedX, int seedY,
                              std::function<Colour(int, int)> getPixelColour,
                              std::function<void(int, int)>   setPixel,
                              Colour boundary,
                              Colour fill)
    {
        // Use a vector as a stack (DFS) — iterative
        std::vector<Point2i> stack;
        stack.push_back({ seedX, seedY });

        while (!stack.empty())
        {
            Point2i cur = stack.back();
            stack.pop_back();

            Colour col = getPixelColour(cur.x, cur.y);
            if (col == boundary || col == fill) continue;

            setPixel(cur.x, cur.y);

            stack.push_back({ cur.x + 1, cur.y });
            stack.push_back({ cur.x - 1, cur.y });
            stack.push_back({ cur.x,     cur.y + 1 });
            stack.push_back({ cur.x,     cur.y - 1 });
        }
    }

}  // namespace FillAlgorithms
