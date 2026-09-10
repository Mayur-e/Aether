#pragma once
// C++17 inline globals — define per-frame timing, no .cpp needed.
namespace Time {
    inline float dt    = 0.016f;   // seconds since last frame (clamped to 50 ms)
    inline float total = 0.0f;     // total elapsed time since game start
}
