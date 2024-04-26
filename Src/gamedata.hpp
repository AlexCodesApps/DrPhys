#pragma once
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace Camera {
    constexpr int16_t width = 400, height = 200;
    int8_t scale = 1; 
    SDL_Point Position;
};

namespace TimeManager {
    static uint64_t ElapsedTime = SDL_GetTicks64();
    static uint64_t DeltaTime = 0;
    void AdvanceTime() {
        DeltaTime = SDL_GetTicks64() - ElapsedTime;
        ElapsedTime = SDL_GetTicks64();
    }
}

enum class SpriteKind {
    None = 0,
    Tile,
    Player,
    Enemy,
    Weapon,
    Orb,
    Acid,
};

typedef SDL_FRect Hitbox;

struct Image {
    SDL_Texture* texture;
    SDL_Rect Source;
};

struct Sprite {
    static inline uint64_t z_counter = 0;
    static inline uint64_t last_id = 0;
    const uint64_t ID = last_id++;
    SpriteKind kind = SpriteKind::None;
    SDL_FRect Body;
    Image * image;
    int depth = z_counter++;
    Sprite(SpriteKind _kind, Hitbox _Body, Image * _image) : kind(_kind), Body(_Body), image(_image) {}
    Sprite(SpriteKind _kind, Image * _image) : kind(_kind), image(_image) {}
    Sprite(SpriteKind = SpriteKind::None) : kind(SpriteKind::None) {}
};

struct Animator {
    bool Active = true;
    Animator(Image * image, int8_t _FPS, bool Active = true) : FPS(_FPS > 0 ? _FPS : 1), image(image), Active(Active) {
        SDL_QueryTexture(image->texture, nullptr, nullptr, &FrameCount, nullptr);
        FrameCount /= Width;
        animators.push_back(this);
    }
    ~Animator() {
        animators.erase(std::remove(animators.begin(), animators.end(), this), animators.end());
    }
    int FrameCount = 0;
    uint8_t Counter = 0;
    uint8_t const FPS = 10;
    static constexpr uint8_t Width = 16;
    static inline std::vector<Animator*> animators;
    static void GlobalUpdate() {
        for (auto animator : animators) {
            animator->Update();
        }
    }
    Image * image;
    void Update() {
        if (!Active) return;
        const int Wait = 1000 / FPS;
        if (++Counter*TimeManager::DeltaTime >= Wait) {
            Counter = 0;
            image->Source.x += Width;
            if (image->Source.x >= FrameCount * Width) image->Source.x = 0;
        }
    }
};


SDL_FRect operator+(const SDL_FRect& a, const SDL_FRect& b) {
    return {a.x + b.x, a.y + b.y, a.w + b.w, a.h + b.h};
}

SDL_FRect operator +(const SDL_FRect& a, const SDL_Point& b) {
    return {a.x + b.x, a.y + b.y, a.w, a.h};
}

namespace Renderer {
    static constexpr int8_t SDL_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    static SDL_Renderer* renderer;
    static SDL_Window* window;
    static std::vector<Sprite*> sprites;
    static SDL_Event event;
    static bool running = true;
    static SDL_Color render_clear = {0, 0, 0, 0};
    void Init() {
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow("DrPhys", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Camera::width * Camera::scale, Camera::height * Camera::scale, SDL_FLAGS);
        renderer = SDL_CreateRenderer(window, -1, 0);
        SDL_SetRenderDrawColor(renderer, render_clear.r, render_clear.g, render_clear.b, render_clear.a);
        SDL_RenderClear(renderer);
    }
    void Update() {}

    void Input() {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }
    void Render() {
        static SDL_FRect DestRect;
        SDL_RenderClear(renderer);
        for (auto sprite : sprites) {
            DestRect = sprite->Body + Camera::Position;
            SDL_RenderCopyF(renderer, sprite->image->texture, &sprite->image->Source, &DestRect);
        }
        SDL_RenderPresent(renderer);
    }

    void Quit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void SortSprites() {
        std::sort(sprites.begin(), sprites.end(), [](Sprite * a, Sprite * b) { return a->depth > b->depth; });
    }
};