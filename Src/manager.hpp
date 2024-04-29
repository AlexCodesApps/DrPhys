#pragma once
#include "gamedata.hpp"
#include <initializer_list>

namespace ContentManager {
    static std::unordered_map<std::string, SDL_Texture*> textures;

    SDL_Texture* Load(const std::string path) {
        std::string name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - path.find_last_of('/') - 1);
        if (textures.find(name) == textures.end()) {
            textures[name] = IMG_LoadTexture(Renderer::renderer, std::string(path).c_str());
            if (textures[name] != nullptr) {
                std::cout << "Loaded " << path << std::endl;
            }
        }
        return textures.at(name);
    }

    void BatchLoad(std::initializer_list<std::string> paths) {
        for (auto path : paths) {
            Load(path);
        }
    }
    
    inline SDL_Texture* Get(const std::string path) {
        return textures.at(path);
    }

    void Clear() {
        for (auto texture : textures) {
            SDL_DestroyTexture(texture.second);
        }
        textures.clear();
    }
}

namespace InputManager {
    static const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);
    static uint8_t past_keyboard_state[SDL_NUM_SCANCODES];
    void CopyCurrentState() {
        std::copy(keyboard_state, &keyboard_state[SDL_NUM_SCANCODES], past_keyboard_state);
    }
}

namespace SpriteManager {
    class PhysicsEngine;
    class Arena {
        friend class PhysicsEngine;
        static inline std::vector<Arena *> arenas;
        static inline int8_t arena_index = 0;
        int8_t index = arena_index++;
        std::unordered_map<uint64_t, std::unique_ptr<Sprite>> sprites;
        public:

        Arena() {
            arenas.push_back(this);
        }

        ~Arena() {
            sprites.clear();
            arenas.erase(std::remove(arenas.begin(), arenas.end(), this), arenas.end());
        }
        Sprite * New() {
            Sprite * sprite = new Sprite();
            sprites[sprite->ID] = std::unique_ptr<Sprite>(sprite);
            return sprite;
        }

        template<typename... Args>
        Sprite * New(Args&&... args) {
            Sprite * sprite = new Sprite(std::forward<Args>(args)...);
            sprites[sprite->ID] = std::unique_ptr<Sprite>(sprite);
            return sprite;
        }

        Sprite * Transfer(Sprite* sprite) {
            sprites[sprite->ID] = std::unique_ptr<Sprite>(sprite);
            return sprites[sprite->ID].get();
        }

        inline Sprite * Get(uint64_t ID) {
            return sprites[ID].get();
        }

        inline void Delete(Sprite * sprite) {
            sprites.erase(sprite->ID);   
        }

        inline void Delete(uint64_t ID) {
            sprites.erase(ID);
        }

        void ForEach(std::function<void(Sprite*)> func, SpriteKind kind = SpriteKind::None) {
            for (const auto& sprite : sprites) {
                if (kind == SpriteKind::None || sprite.second->kind == kind) {
                    func(sprite.second.get());
                }
            }
        }

        void SwapRenderList() {
            Renderer::sprites.clear();
            for (auto& sprite : sprites) {
                Renderer::sprites.push_back(sprite.second.get());
            }
        }

        void Clear(SpriteKind kind = SpriteKind::None) {
            for (auto& sprite : sprites) {
                if (kind == SpriteKind::None || sprite.second->kind == kind) {
                    Delete(sprite.first);
                }
            }
        }
    };
}