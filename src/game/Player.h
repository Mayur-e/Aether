#pragma once
#include <glm/glm.hpp>
#include "Physics2D.h"
#include "GravitySystem.h"

class Player {
public:
    int bodyId = -1;
    
    // Physics tuning
    float moveAccel = 100.0f;
    float moveDeccel = 50.0f;
    float maxRunSpeed = 12.0f;
    float jumpForce = 18.0f;
    
    void init(Physics2D& phys, glm::vec2 startPos);
    
    // Handle input, apply forces
    void update(float dt, Physics2D& phys, GravitySystem& grav);
};
