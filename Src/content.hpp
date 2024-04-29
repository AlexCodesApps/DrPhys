#pragma once
#include "manager.hpp"
#include "tilemap.hpp"
#include <filesystem>
#include <numeric>
#include <cmath>

enum class SpriteFlags : uint8_t {
    None = 0,
    Ghost = 1 << 0,
    Hitbox = 1 << 1,
    Bouncy = 1 << 2,
    Physics = 1 << 3,
};

bool operator&(const SpriteFlags& a, const SpriteFlags& b) {
    return (uint8_t)a & (uint8_t)b;
}

bool operator&(const uint8_t& a, const SpriteFlags& b) {
    return a & (uint8_t)b;
}

bool operator&(const SpriteFlags& a, const uint8_t& b) {
    return (uint8_t)a & b;
}

bool operator|(const SpriteFlags& a, const SpriteFlags& b) {
    return (uint8_t)a | (uint8_t)b;
}

bool operator|(const uint8_t& a, const SpriteFlags& b) {
    return a | (uint8_t)b;
}
namespace ContentManager {
    static std::vector<Image *> DynamicImages;
    static Image player_front;
    static Image player_back;
    static Image player_left;
    static Image player_right;
    static uint8_t player_anim_fps = 100;
    static Animator player_front_anim = Animator(&player_front, player_anim_fps, true);
    static Animator player_back_anim = Animator(&player_back, player_anim_fps, true);
    static Animator player_left_anim = Animator(&player_left, player_anim_fps, true);
    static Animator player_right_anim = Animator(&player_right, player_anim_fps, true);
    static Tiles::TilesIndex tiles;
    void LoadContent(std::string path);

    Image * DynamicLoad(std::string name, SDL_Rect source = {0, 0, 16, 16}) {
        Image * image = new Image(ContentManager::Get(name), source);
        DynamicImages.push_back(image);
        return image;
    }

    void LoadContent() {
        std::string path = "../Assets/";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() == ".png" || entry.path().extension() == ".bmp") {
                ContentManager::Load(entry.path().string());
            }
            if (entry.is_directory()) {
                LoadContent(entry.path().string());
            }
        }
        player_front = Image(ContentManager::Get("front"));
        player_back = Image(ContentManager::Get("back"));
        player_left = Image(ContentManager::Get("left"));
        player_right = Image(ContentManager::Get("right"));
        player_front_anim.Init(true);
        player_back_anim.Init(true);
        player_left_anim.Init(true);
        player_right_anim.Init(true);
        Tiles::Tile::MakeTileTemplate("ld", ContentManager::DynamicLoad("ld"), (uint8_t)TileFlags::None);
        Tiles::Tile::MakeTileTemplate("dl", ContentManager::DynamicLoad("dl"), (uint8_t)TileFlags::None);
        Tiles::Tile::MakeTileTemplate("wall", ContentManager::DynamicLoad("wall"), (uint8_t)TileFlags::Solid, {0, 0, 16, 8});
        Tiles::Tile::MakeTileTemplate("black", ContentManager::DynamicLoad("black"), (uint8_t)TileFlags::Solid);
        tiles[0] = Tiles::Tile::GetTileTemplate("ld");
        tiles[1] = Tiles::Tile::GetTileTemplate("dl");
        tiles[2] = Tiles::Tile::GetTileTemplate("wall");
        auto ptr = Tiles::TileMapManager::New(new Tiles::TileMap(tiles, 4, 4));
        ptr->LoadTileMap({
            2, 1, 0, 1,
            2, 0, 1, 0, 
            2, 1, 0, 1,
            2, 0, 1, 0
        });
        Tiles::TileMapManager::index = ptr->index;
    }

    void LoadContent(std::string recursive_path) {
        for (const auto & entry : std::filesystem::directory_iterator(recursive_path)) {
            if (entry.path().extension() == ".png" || entry.path().extension() == ".bmp") {
                ContentManager::Load(entry.path().string());
            }
            if (entry.is_directory()) {
                LoadContent(entry.path().string());
            }
        }
    }

    void ClearDynamicImages() {
        for (auto image : DynamicImages) {
            delete image;
        }
        DynamicImages.clear();
    }
}

namespace Math {
    auto MapNumberFromRangeToRange(auto number, auto fromMin, auto fromMax, auto toMin, auto toMax) {
        return ((number - fromMin) / (fromMax - fromMin)) * (toMax - toMin) + toMin;
    }
    SDL_FPoint ResolveCollison(const Hitbox & source, const Hitbox & target, const SDL_FPoint & movementOp) {
        SDL_FRect translatedPosition = {movementOp.x, movementOp.y, source.w, source.h};
        if (!SDL_HasIntersectionF(&target, &translatedPosition)) return movementOp;
        else {
            SDL_FRect offset = {source.x - target.x, source.y - target.y};
            offset = {offset.x / target.w, offset.y / target.h};
            return {movementOp.x + offset.x, movementOp.y + offset.y};
        }
    }
}