#include "content.hpp"

namespace GameState {
    static bool paused = false;
    static bool running = true;
    static SpriteManager::Arena arena;
    static float player_speed = 150.0f;
    static Sprite * player = nullptr;
    std::vector<std::pair<uint64_t, SDL_FPoint>> MovementQueue;
    void Init() {
        player = arena.New(SpriteKind::Player);
        auto foo = arena.New(SpriteKind::Enemy);
        foo->Body = {-20, -20, 16, 16};
        foo->image = ContentManager::DynamicLoad("white");
        player->Body = {0, 0, 16, 16};
        player->image = &ContentManager::player_front;
        arena.SwapRenderList();
    }

    void Update() {
        if (paused) return;
        if (MovementQueue.size()) {
            for (auto& move : MovementQueue) {
                auto tilemap = Tiles::TileMapManager::Get();
                if (tilemap) tilemap->ForEach([&](Tiles::Tile& tile) {
                    if (tile.Flags & (uint8_t)TileFlags::Solid) {
                        move.second = Math::ResolveCollison(arena.Get(move.first)->Body, tile.hitbox, move.second);
                    }
                });
                arena.ForEach([&](Sprite * sprite) {
                    if (sprite->ID == move.first) return;
                    move.second = Math::ResolveCollison(arena.Get(move.first)->Body, sprite->Body, move.second);
                });
                arena.Get(move.first)->Body.x = move.second.x;
                arena.Get(move.first)->Body.y = move.second.y;
            }
            MovementQueue.clear();
        }
    }

    void Input() {
        if (paused) return;
        SDL_FPoint movement_axis = {0, 0};
        if (InputManager::keyboard_state[SDL_SCANCODE_UP]) movement_axis.y -= 1;
        if (InputManager::keyboard_state[SDL_SCANCODE_DOWN]) movement_axis.y += 1;
        if (InputManager::keyboard_state[SDL_SCANCODE_LEFT]) movement_axis.x -= 1;
        if (InputManager::keyboard_state[SDL_SCANCODE_RIGHT]) movement_axis.x += 1;
        if (!movement_axis.x && !movement_axis.y) return;
        float rad = std::atan2(movement_axis.y, movement_axis.x);
        SDL_FPoint target = {std::cos(rad) * player_speed * (float)TimeManager::DeltaTime + player->Body.x, std::sin(rad) * player_speed * (float)TimeManager::DeltaTime + player->Body.y};
        MovementQueue.push_back({player->ID, target});
    }
}