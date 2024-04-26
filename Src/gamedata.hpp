#pragma once
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <vector>
#include <unordered_map>
#include <string>

namespace Camera {
    constexpr int16_t width = 400, height = 200;
    int8_t scale = 1; 
    SDL_Point Position;
};

enum class SpriteKind {
    None = 0,
    Player,
    Enemy,
    Weapon,
    Orb,
    Acid,
};

typedef SDL_FRect Hitbox;

struct Sprite {
    SpriteKind kind = SpriteKind::None;
    int64_t depth;
    SDL_Texture* image;
    SDL_Rect Source;
    SDL_FRect Body;
};

enum class Direction {
    None = 0,
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    DownRight
};

struct AnimatedSprite {
    AnimatedSprite() {
        animated_sprites.push_back(this);
    }
    AnimatedSprite(Sprite base, int frameCount, double fps) : base_sprite(base), frameCount(frameCount), FPS(fps) {
        animated_sprites.push_back(this);
    }
    static inline std::vector<AnimatedSprite*> animated_sprites;
    double FPS = 5.5;
    int currentFrame = 0;
    int frameCount = 1;
    int width = 16, height = 16;
    Sprite base_sprite;
    void AdvanceFrame() {
        currentFrame = (currentFrame < frameCount - 1) ? currentFrame + 1 : 0;
        base_sprite.Source.x = currentFrame * width;
    }

    void Update() {
        static Uint64 elapsedTime = 0.0;
        Uint64 currentTime = SDL_GetTicks64();
        if (currentTime - elapsedTime >= 1000.0 / FPS) {
            AdvanceFrame();
            elapsedTime = currentTime;
        }
    }

    static void GlobalUpdate() {
        for (auto& sprite : animated_sprites) {
            sprite->Update();
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
    static std::vector<Sprite*> RenderList;
    static SDL_Renderer* renderer;
    static SDL_Window* window;
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
        static const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }
    void Render() {
        SDL_RenderClear(renderer);
        for (auto * sprite : RenderList) {
            auto bufferedRect = sprite->Body + Camera::Position;
            SDL_RenderCopyF(renderer, sprite->image, &sprite->Source, &bufferedRect);
        }
        SDL_RenderPresent(renderer);
    }

    void Quit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};