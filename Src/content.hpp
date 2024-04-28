#pragma once
#include "manager.hpp"
#include "tilemap.hpp"

enum class SpriteFlags : uint8_t {
    None = 0,
    Ghost = 1 << 0,
    Hitbox = 1 << 1,
    Bouncy = 1 << 2,
    Physics = 1 << 3,
    PhysicsFlags = 1 << 4,
};

struct SpritePhysicsData {
    Sprite * sprite;
    Hitbox hitbox;
    float ax = 0, ay = 0, vx = 0, vy = 0;
    uint8_t flags = (uint8_t)SpriteFlags::None;
};

void LoadContent() {
    TextureManager::BatchLoad({
        "back.png",
        "left.png",
        "right.png",
        "front.png",
        "white.png",
        "death.png",
        "Tiles/dl.bmp", 
        "Tiles/ld.bmp",
        "Tiles/Wall.bmp"});
}

namespace SpriteManager {
    class PhysicsEngine {
    // NEED TILEMAP POINTER
    SpriteManager::Arena * arena;
    public:
        void SetArena(SpriteManager::Arena * _arena) { arena = _arena;}
        void Update() {}
    };
}