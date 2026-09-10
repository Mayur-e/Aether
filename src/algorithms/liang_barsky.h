#pragma once

// -----------------------------------------------------------------------
// Liang-Barsky parametric line clipping algorithm.
//
// More efficient than Cohen-Sutherland for general lines because it
// computes parametric intersections directly.
//
// Clips line P1→P2 against window [xMin,xMax]×[yMin,yMax].
// Returns true if visible; updates t1 and t2 (parameters 0..1 along the line).
// Caller can recover clipped endpoints as:
//   cx1 = x1 + t1*(x2-x1),  cy1 = y1 + t1*(y2-y1)
//   cx2 = x1 + t2*(x2-x1),  cy2 = y1 + t2*(y2-y1)
//
// Reference: Liang & Barsky, "A New Concept and Method for Line Clipping" (1984)
// -----------------------------------------------------------------------

namespace LiangBarsky
{
    struct Window { float xMin, yMin, xMax, yMax; };

    // Helper: update the parameter interval [t1, t2] for one edge.
    // p: direction component (negative = entering, positive = leaving)
    // q: distance to edge.
    // Returns false if the line is trivially rejected.
    inline bool clipTest(float p, float q, float& t1, float& t2)
    {
        if (p == 0.0f)
            return (q >= 0.0f);       // parallel & outside → reject

        float r = q / p;
        if (p < 0.0f)
        {
            if (r > t2) return false;
            if (r > t1) t1 = r;       // entering side
        }
        else
        {
            if (r < t1) return false;
            if (r < t2) t2 = r;       // leaving side
        }
        return true;
    }

    // Clip the line (x1,y1)→(x2,y2) against window w.
    // Returns true if any portion is visible.
    // cx1,cy1 and cx2,cy2 receive the clipped endpoints.
    inline bool clip(float x1, float y1, float x2, float y2, const Window& w,
                     float& cx1, float& cy1, float& cx2, float& cy2)
    {
        float dx = x2 - x1;
        float dy = y2 - y1;

        float t1 = 0.0f, t2 = 1.0f;

        if (!clipTest(-dx, x1 - w.xMin, t1, t2)) return false;
        if (!clipTest( dx, w.xMax - x1, t1, t2)) return false;
        if (!clipTest(-dy, y1 - w.yMin, t1, t2)) return false;
        if (!clipTest( dy, w.yMax - y1, t1, t2)) return false;

        cx1 = x1 + t1 * dx;
        cy1 = y1 + t1 * dy;
        cx2 = x1 + t2 * dx;
        cy2 = y1 + t2 * dy;
        return true;
    }

}  // namespace LiangBarsky
