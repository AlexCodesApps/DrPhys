#pragma once
#include "gamedata.hpp"

enum class TileFlags : uint8_t {
    None = 0,
    Solid = 1 << 0,
    Death = 1 << 1,
};

namespace Tiles {
    struct Tile {
        static inline std::unordered_map<std::string, Tile> tiles;
        static constexpr uint8_t width = 16, height = 16;
        uint8_t Flags = (uint8_t)TileFlags::None;
        Image * image = nullptr;

        static Tile MakeTileTemplate(std::string name, Image * image, uint8_t flags = (uint8_t)TileFlags::None) {
            tiles[name] = Tile{.Flags = flags, .image = image};
            return tiles[name];
        }
        static inline Tile GetTileTemplate(std::string name) {
            return tiles.at(name);
        }

        static Sprite GetSpriteTemplate(std::string name) {
            Tile tile = GetTileTemplate(name);
            return Sprite(SpriteKind::Tile, tile.image);
        }
    };

    typedef std::unordered_map<uint64_t, Tile> TilesIndex;

    struct TileMap {
        static inline int tilemap_index = 0;
        SDL_FPoint Position;
        float scale = 1;
        const int index = tilemap_index++;
        static inline std::vector<TileMap*> tilemaps;
        TilesIndex tileindex;
        std::vector<Tile> tiles;
        std::vector<int8_t> tilemap;
        int width = 0, height = 0;
        TileMap(const TilesIndex & _tileindex, int _width, int _height) : width(_width), height(_height), tileindex(_tileindex) {}

        void LoadTileMap(std::vector<int8_t> _tilemap) {
            tilemap = _tilemap;
        }

        void Draw() {
            int total = tilemap.size();
            for (int i = 0; i < total; i++) {
                int x_offset = i % width;
                int y_offset = (i-x_offset) / width;
                Tile tile = tileindex.at(tilemap[i]);
                if (tile.image == nullptr) continue;
                Sprite tile_sprite = Sprite(SpriteKind::Tile, tile.image);
                tile_sprite.Body = {x_offset * width * scale + Position.x, y_offset * height * scale + Position.y, Tile::width * scale, Tile::height * scale};
                tile_sprite.depth = -1;
                Renderer::Draw(&tile_sprite);
            }
        }
    };

    namespace TileMapManager {
        static int index = -1;
        static std::unordered_map<int, TileMap*> tilemaps;
        void Draw() {
            if (index < 0) return;
            tilemaps.at(index)->Draw();
        }

        void New(TileMap * tilemap) {
            tilemaps[tilemap->index] = tilemap;
        }

        void Remove(int index) {
            tilemaps.erase(index);
        }

        void Remove(TileMap * tilemap) {
            tilemaps.erase(tilemap->index);
        }
    };
}