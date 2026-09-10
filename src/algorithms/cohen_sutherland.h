#pragma once

// -----------------------------------------------------------------------
// Cohen-Sutherland line clipping algorithm.
//
// Clips a line segment against an axis-aligned rectangular window.
// Returns true if any part of the line is visible; modifies (x1,y1)->(x2,y2)
// to the visible portion.
//
// Reference: Foley, van Dam et al., "Computer Graphics: Principles and Practice"
// -----------------------------------------------------------------------

namespace CohenSutherland
{
    // Region codes
    const int INSIDE = 0;  // 0000
    const int LEFT   = 1;  // 0001
    const int RIGHT  = 2;  // 0010
    const int BOTTOM = 4;  // 0100
    const int TOP    = 8;  // 1000

    // Clip window bounds
    struct Window { float xMin, yMin, xMax, yMax; };

    // Compute the region code for point (x, y) relative to window
    inline int computeCode(float x, float y, const Window& w)
    {
        int code = INSIDE;
        if      (x < w.xMin) code |= LEFT;
        else if (x > w.xMax) code |= RIGHT;
        if      (y < w.yMin) code |= BOTTOM;
        else if (y > w.yMax) code |= TOP;
        return code;
    }

    // Cohen-Sutherland clip. Returns true if the line is (at least partially) visible.
    // (x1,y1) and (x2,y2) are modified to the clipped endpoints.
    inline bool clip(float& x1, float& y1, float& x2, float& y2, const Window& w)
    {
        int code1 = computeCode(x1, y1, w);
        int code2 = computeCode(x2, y2, w);

        while (true)
        {
            if (!(code1 | code2))       // both inside
                return true;

            if (code1 & code2)          // both outside same region
                return false;

            // Pick the outside point
            int codeOut = code1 ? code1 : code2;

            float x, y;

            if (codeOut & TOP)
            {
                x = x1 + (x2 - x1) * (w.yMax - y1) / (y2 - y1);
                y = w.yMax;
            }
            else if (codeOut & BOTTOM)
            {
                x = x1 + (x2 - x1) * (w.yMin - y1) / (y2 - y1);
                y = w.yMin;
            }
            else if (codeOut & RIGHT)
            {
                y = y1 + (y2 - y1) * (w.xMax - x1) / (x2 - x1);
                x = w.xMax;
            }
            else  // LEFT
            {
                y = y1 + (y2 - y1) * (w.xMin - x1) / (x2 - x1);
                x = w.xMin;
            }

            if (codeOut == code1)
            {
                x1 = x; y1 = y;
                code1 = computeCode(x1, y1, w);
            }
            else
            {
                x2 = x; y2 = y;
                code2 = computeCode(x2, y2, w);
            }
        }
    }

}  // namespace CohenSutherland
