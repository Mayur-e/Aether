#include "Level.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// -----------------------------------------------------------------------
// Helper: add a static wall geometry + physics body.
// -----------------------------------------------------------------------
void Level::addWall(float x, float y, float w, float h,
                    float r, float g, float b)
{
    Geometry wall;
    wall.pos   = { x, y };
    wall.size  = { w, h };
    wall.color = { r, g, b };
    walls.push_back(wall);

    Body2D body;
    body.pos  = { x, y };
    body.half = { w * 0.5f, h * 0.5f };
    body.mass = 0.0f;   // static
    physics.addBody(body);
}

// -----------------------------------------------------------------------
// loadFromFile — parse a level text file.
//
// Supported tokens (one per line, # = comment):
//   NAME <name (underscores → spaces)>
//   SPAWN <x> <y>
//   CHECKPOINT <x> <y>
//   GRAVITY_SCALE <s>         -- Level 5 low-gravity mode
//   WALL <x> <y> <w> <h> [r g b]
//   CUBE <x> <y> <mass>
//   DOOR <name> <cx> <cy> <ox> <oy> <w> <h>
//   PLATE <signalId> <x> <y>  -- pressure plate
//   BUTTON <signalId> <x> <y> -- alias for PLATE
//   SWITCH <dir> <x> <y>      -- gravity switch (dir = UP/DOWN/LEFT/RIGHT)
//   FIELD <dir> <strength> <x> <y> <w> <h>
//   VALVE <signalId> <x> <y>  -- conduit valve (one-shot switch)
//   GOAL <x> <y>              -- exit portal
//   SPIKE <x> <y> <w> <h>     -- hazard tile
//   SENTINEL <x1> <y1> <x2> <y2> [speed]
//   WRAITH <cx> <cy> <radius> [speed]
//   TEXT <x> <y> <message...>
// -----------------------------------------------------------------------
void Level::loadFromFile(const std::string& path) {
    filePath_ = path;

    // Clear all state
    physics.clear();
    walls.clear();
    plates.clear();
    doors.clear();
    cubes.clear();
    switches.clear();
    fields.clear();
    valves.clear();
    goals.clear();
    spikes.clear();
    enemies.clear();
    texts.clear();
    isComplete = false;
    gravityScale = 1.0f;

    // Reset gravity to DOWN
    gravity = GravitySystem();

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << path << "\n";
        buildHardcodedLevel1();
        checkpointPos = spawnPoint;
        player.init(physics, spawnPoint);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        // ---- Meta ----
        if (token == "NAME") {
            ss >> name;
            for (auto& c : name) if (c == '_') c = ' ';
        }
        else if (token == "SPAWN") {
            ss >> spawnPoint.x >> spawnPoint.y;
            checkpointPos = spawnPoint;
        }
        else if (token == "CHECKPOINT") {
            ss >> checkpointPos.x >> checkpointPos.y;
        }
        else if (token == "GRAVITY_SCALE") {
            ss >> gravityScale;
        }

        // ---- Static Geometry ----
        else if (token == "WALL") {
            float x, y, w, h;
            float r = 0.15f, g_ = 0.15f, b = 0.20f;
            ss >> x >> y >> w >> h;
            ss >> r >> g_ >> b;
            addWall(x, y, w, h, r, g_, b);
        }

        // ---- Movable Cube ----
        else if (token == "CUBE") {
            float x, y, mass = 2.0f;
            ss >> x >> y >> mass;
            GravityCube cube;
            cube.mass = mass;
            Body2D b;
            b.pos  = { x, y };
            b.half = { 0.5f, 0.5f };
            b.mass = mass;
            cube.bodyId = physics.addBody(b);
            cubes.push_back(cube);
        }

        // ---- Door ----
        else if (token == "DOOR") {
            Door door;
            float w, h;
            ss >> door.name
               >> door.closedPos.x >> door.closedPos.y
               >> door.openPos.x   >> door.openPos.y
               >> w >> h;
            Body2D b;
            b.pos  = door.closedPos;
            b.half = { w * 0.5f, h * 0.5f };
            b.mass = 0.0f;
            door.bodyId = physics.addBody(b);
            doors.push_back(door);
        }

        // ---- Pressure Plate (PLATE or legacy BUTTON) ----
        else if (token == "PLATE" || token == "BUTTON") {
            PressurePlate plate;
            float x, y;
            ss >> plate.signalId >> x >> y;
            Body2D b;
            b.pos     = { x, y };
            b.half    = { 0.6f, 0.08f };
            b.mass    = 0.0f;
            b.noGravity = true;
            plate.bodyId = physics.addBody(b);
            plates.push_back(plate);
        }

        // ---- Gravity Switch ----
        else if (token == "SWITCH") {
            std::string dirStr;
            float x, y;
            ss >> dirStr >> x >> y;

            GravitySwitch sw;
            if      (dirStr == "UP")    sw.direction = GravityDir::UP;
            else if (dirStr == "DOWN")  sw.direction = GravityDir::DOWN;
            else if (dirStr == "LEFT")  sw.direction = GravityDir::LEFT;
            else if (dirStr == "RIGHT") sw.direction = GravityDir::RIGHT;

            Body2D b;
            b.pos     = { x, y };
            b.half    = { 0.6f, 1.2f };
            b.mass    = 0.0f;
            b.noGravity = true;
            sw.bodyId = physics.addBody(b);
            switches.push_back(sw);
        }

        // ---- Gravity Field ----
        else if (token == "FIELD") {
            std::string dirStr;
            float strength, x, y, w, h;
            ss >> dirStr >> strength >> x >> y >> w >> h;

            GravityField field;
            if      (dirStr == "UP")    field.direction = GravityDir::UP;
            else if (dirStr == "DOWN")  field.direction = GravityDir::DOWN;
            else if (dirStr == "LEFT")  field.direction = GravityDir::LEFT;
            else if (dirStr == "RIGHT") field.direction = GravityDir::RIGHT;
            field.strength = strength;
            field.pos  = { x, y };
            field.half = { w * 0.5f, h * 0.5f };
            fields.push_back(field);
        }

        // ---- Conduit Valve ----
        else if (token == "VALVE") {
            ConduitValve valve;
            float x, y;
            ss >> valve.signalId >> x >> y;
            Body2D b;
            b.pos     = { x, y };
            b.half    = { 0.7f, 1.2f };
            b.mass    = 0.0f;
            b.noGravity = true;
            valve.bodyId = physics.addBody(b);
            valves.push_back(valve);
        }

        // ---- Goal / Exit ----
        else if (token == "GOAL") {
            float x, y;
            ss >> x >> y;
            Goal goal;
            Body2D b;
            b.pos     = { x, y };
            b.half    = { 0.9f, 1.8f };
            b.mass    = 0.0f;
            b.noGravity = true;
            goal.bodyId = physics.addBody(b);
            goals.push_back(goal);
        }

        // ---- Spike Hazard ----
        else if (token == "SPIKE") {
            float x, y, w, h;
            ss >> x >> y >> w >> h;
            Spike spike;
            Body2D b;
            b.pos     = { x, y };
            b.half    = { w * 0.5f, h * 0.5f };
            b.mass    = 0.0f;
            b.noGravity = true;
            spike.bodyId = physics.addBody(b);
            spikes.push_back(spike);
        }

        // ---- Sentinel Enemy ----
        else if (token == "SENTINEL") {
            float x1, y1, x2, y2, spd = 4.0f;
            ss >> x1 >> y1 >> x2 >> y2 >> spd;
            Enemy e;
            e.type    = EnemyType::SENTINEL;
            e.patrolA = { x1, y1 };
            e.patrolB = { x2, y2 };
            e.speed   = spd;
            Body2D b;
            b.pos     = { x1, y1 };
            b.half    = { 0.5f, 0.9f };
            b.mass    = 0.0f;    // kinematic — driven by patrol code
            b.noGravity = true;
            e.bodyId  = physics.addBody(b);
            enemies.push_back(e);
        }

        // ---- Wraith Enemy ----
        else if (token == "WRAITH") {
            float cx, cy, radius, spd = 1.2f;
            ss >> cx >> cy >> radius >> spd;
            Enemy e;
            e.type        = EnemyType::WRAITH;
            e.orbitCenter = { cx, cy };
            e.orbitRadius = radius;
            e.orbitSpeed  = spd;
            Body2D b;
            b.pos     = { cx + radius, cy };
            b.half    = { 0.6f, 0.6f };
            b.mass    = 0.0f;
            b.noGravity = true;
            e.bodyId  = physics.addBody(b);
            enemies.push_back(e);
        }

        // ---- Tutorial Text ----
        else if (token == "TEXT") {
            TextHint th;
            ss >> th.pos.x >> th.pos.y;
            std::getline(ss, th.text);
            if (!th.text.empty() && th.text[0] == ' ') th.text = th.text.substr(1);
            texts.push_back(th);
        }
    }

    // Apply gravity scale (Level 5 low-g)
    // GravitySystem::STRENGTH is a compile-time constant; we'll scale the
    // physics world gravity directly after GravitySystem sets it each frame.
    // Store the scale and apply in update().

    checkpointPos = spawnPoint; // reset checkpoint to spawn on fresh load
    player.init(physics, spawnPoint);
}

// -----------------------------------------------------------------------
void Level::respawnPlayer() {
    if (player.bodyId >= 0) {
        Body2D& pb = physics.body(player.bodyId);
        pb.pos = checkpointPos;
        pb.vel = { 0.0f, 0.0f };
    }
    // Reset gravity to DOWN on death
    gravity = GravitySystem();
}

// -----------------------------------------------------------------------
void Level::buildHardcodedLevel1() {
    name       = "Level 1: Arrival";
    spawnPoint = { 1.0f, 2.0f };

    addWall( 0.0f, 0.0f, 32.0f, 1.0f);   // floor
    addWall(10.0f, 3.0f,  6.0f, 1.0f);   // raised ledge
    addWall(18.0f, 6.0f,  6.0f, 1.0f);   // higher ledge
    addWall(22.0f,10.0f, 10.0f, 1.0f);   // ceiling section

    // Level bounds
    addWall(-1.0f, 6.0f, 2.0f, 14.0f);
    addWall(33.0f, 6.0f, 2.0f, 14.0f);

    // Gravity switches
    GravitySwitch swUp, swDown;
    swUp.direction = GravityDir::UP;
    swDown.direction = GravityDir::DOWN;

    Body2D bUp; bUp.pos = {14.0f, 4.5f}; bUp.half = {0.6f, 1.2f}; bUp.mass = 0.0f; bUp.noGravity = true;
    swUp.bodyId = physics.addBody(bUp);
    switches.push_back(swUp);

    Body2D bDown; bDown.pos = {24.0f, 11.5f}; bDown.half = {0.6f, 1.2f}; bDown.mass = 0.0f; bDown.noGravity = true;
    swDown.bodyId = physics.addBody(bDown);
    switches.push_back(swDown);

    // Goal
    Goal goal;
    Body2D bg; bg.pos = {30.0f, 1.9f}; bg.half = {0.9f, 1.8f}; bg.mass = 0.0f; bg.noGravity = true;
    goal.bodyId = physics.addBody(bg);
    goals.push_back(goal);

    texts.push_back({{1.0f,  3.0f}, "Use A/D to move, Space to jump"});
    texts.push_back({{10.0f, 5.0f}, "Walk into the switch to flip Gravity!"});
    texts.push_back({{22.0f,12.0f}, "Step into the arch to exit the level"});
}

// -----------------------------------------------------------------------
void Level::update(float dt) {
    gravity.update(dt);

    // Apply gravity scale for low-gravity levels (e.g. Level 5)
    physics.gravity = gravity.vector() * gravityScale;

    // Apply per-zone gravity field overrides to bodies
    applyGravityFields(fields, physics, gravity);

    player.update(dt, physics, gravity);

    // Update enemies
    bool killedByEnemy = updateEnemies(enemies, physics, player.bodyId, dt);

    // Update puzzle objects
    bool killedBySpike = false;
    bool goalReached   = updatePuzzleObjects(
        plates, doors, cubes, switches, valves, goals, spikes,
        player, physics, gravity, killedBySpike, dt);

    if (killedByEnemy || killedBySpike) {
        respawnPlayer();
    }

    physics.step(dt);

    if (goalReached) isComplete = true;
}

// -----------------------------------------------------------------------
// Colour helpers for switch directions
static glm::vec3 switchColor(GravityDir d) {
    switch (d) {
        case GravityDir::DOWN:  return { 0.2f, 0.5f, 1.0f };   // blue
        case GravityDir::UP:    return { 0.8f, 0.3f, 0.9f };   // purple
        case GravityDir::LEFT:  return { 0.3f, 0.9f, 0.4f };   // green
        case GravityDir::RIGHT: return { 0.9f, 0.6f, 0.1f };   // amber
    }
    return { 1.0f, 1.0f, 1.0f };
}

static glm::vec3 fieldColor(GravityDir d) {
    return switchColor(d) * 0.6f;
}

// -----------------------------------------------------------------------
void Level::draw(Renderer& renderer, const GameCamera& camera) {
    if (!cubeMesh) return;

    auto submit = [&](glm::vec2 pos, glm::vec2 sz, glm::vec3 col, float gravAngle = 0.0f) {
        DrawCommand cmd;
        cmd.mesh = cubeMesh;
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
        if (gravAngle != 0.0f)
            m = glm::rotate(m, gravAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, glm::vec3(sz.x, sz.y, 1.0f));
        cmd.modelMatrix = m;
        cmd.colour      = col;
        renderer.submit(cmd);
    };

    // Walls
    for (const auto& w : walls)
        submit(w.pos, w.size, w.color);

    // Gravity Fields (shown as translucent coloured zones)
    for (const auto& f : fields)
        submit(f.pos, f.half * 2.0f, fieldColor(f.direction) * 0.4f);

    // Gravity Switches
    for (const auto& sw : switches) {
        if (sw.bodyId < 0) continue;
        const Body2D& sb = physics.body(sw.bodyId);
        glm::vec3 col = switchColor(sw.direction);
        // Pillar base
        submit(sb.pos - glm::vec2(0.0f, 0.6f), { 0.4f, 1.2f }, col * 0.7f);
        // Arrow head (smaller bright quad on top)
        submit(sb.pos + glm::vec2(0.0f, 0.5f), { 0.5f, 0.5f }, col);
    }

    // Pressure Plates
    for (const auto& p : plates) {
        if (p.bodyId < 0) continue;
        const Body2D& pb = physics.body(p.bodyId);
        glm::vec3 col = p.isPressed
            ? glm::vec3(0.9f, 0.7f, 0.1f)   // amber – active
            : glm::vec3(0.3f, 0.3f, 0.3f);  // grey – inactive
        submit(pb.pos, pb.half * 2.0f, col);
    }

    // Gravity Cubes
    for (const auto& c : cubes) {
        if (c.bodyId < 0) continue;
        const Body2D& cb = physics.body(c.bodyId);
        submit(cb.pos, cb.half * 2.0f, { 0.4f, 0.2f, 0.7f }); // deep violet
    }

    // Doors
    for (const auto& d : doors) {
        if (d.bodyId < 0) continue;
        const Body2D& db = physics.body(d.bodyId);
        float t = d.transition;
        glm::vec3 col = glm::mix(glm::vec3(0.2f, 0.5f, 0.8f),
                                  glm::vec3(0.1f, 0.9f, 0.4f), t);
        submit(db.pos, db.half * 2.0f, col);
    }

    // Conduit Valves
    for (const auto& v : valves) {
        if (v.bodyId < 0) continue;
        const Body2D& vb = physics.body(v.bodyId);
        glm::vec3 col = v.activated
            ? glm::vec3(0.0f, 0.9f, 1.0f)   // bright cyan
            : glm::vec3(0.3f, 0.3f, 0.35f); // dim grey
        submit(vb.pos, { 0.5f, 1.2f }, col);
    }

    // Goals (exit arches – pulsing white/violet)
    for (const auto& g : goals) {
        if (g.bodyId < 0) continue;
        const Body2D& gb = physics.body(g.bodyId);
        submit(gb.pos, gb.half * 2.0f, { 0.5f, 0.8f, 1.0f });
    }

    // Spikes (red hazard tiles)
    for (const auto& s : spikes) {
        if (s.bodyId < 0) continue;
        const Body2D& sb = physics.body(s.bodyId);
        submit(sb.pos, sb.half * 2.0f, { 1.0f, 0.1f, 0.1f });
    }

    // Enemies
    for (const auto& e : enemies) {
        if (!e.active || e.bodyId < 0) continue;
        const Body2D& eb = physics.body(e.bodyId);
        glm::vec3 col = (e.type == EnemyType::SENTINEL)
            ? glm::vec3(0.9f, 0.3f, 0.1f)   // orange-red Sentinel
            : glm::vec3(0.6f, 0.0f, 0.8f);  // purple Wraith
        submit(eb.pos, eb.half * 2.0f, col);
    }

    // Player
    if (player.bodyId >= 0) {
        const Body2D& pb = physics.body(player.bodyId);
        DrawCommand cmd;
        cmd.mesh = cubeMesh;
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(pb.pos, 0.0f));
        m = glm::rotate(m, gravity.angle(), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, glm::vec3(pb.half.x * 2.0f, pb.half.y * 2.0f, 0.5f));
        cmd.modelMatrix = m;
        cmd.colour      = { 0.2f, 0.8f, 1.0f }; // cyan
        renderer.submit(cmd);
    }
}
