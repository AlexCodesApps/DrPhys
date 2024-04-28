#pragma once
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

typedef SDL_Point Dimensions;

namespace Camera {
    static int16_t width = 400, height = 200;
    float scale = 1; 
    SDL_Point Position;
    Dimensions GetScreenSize() {
        SDL_Rect store;
        if (SDL_GetDisplayBounds(0, &store) >= 0) return Dimensions{store.w, store.h};
        else std::cerr << SDL_GetError() << std::endl;
        return {0, 0};
    }
};

namespace TimeManager {
    static uint64_t ElapsedTime = SDL_GetTicks64();
    static double DeltaTime = 0;
    void AdvanceTime() {
        DeltaTime = (SDL_GetTicks64() - ElapsedTime);
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

struct Hitbox {
    float w = 0, h = 0;
};

struct Image {
    SDL_Texture* texture;
    SDL_Rect Source;
};

class Sprite {
    static inline uint64_t z_counter = 0;
    static inline uint64_t last_id = 0;
    public:
    const uint64_t ID = last_id++;
    SpriteKind kind = SpriteKind::None;
    SDL_FRect Body;
    Image * image;
    int depth = z_counter++;
    Sprite(SpriteKind _kind, SDL_FRect _Body, Image * _image) : kind(_kind), Body(_Body), image(_image) {}
    Sprite(SpriteKind _kind, Image * _image) : kind(_kind), image(_image) {}
    Sprite(SpriteKind = SpriteKind::None) : kind(SpriteKind::None) {}
};

struct Animator {
    bool Active = false;
    Animator() = default;
    Animator(Image * image, int8_t _FPS, bool Active = true, bool _register = false) : FPS(_FPS > 0 ? _FPS : 1), image(image), Active(Active) {
        SDL_QueryTexture(image->texture, nullptr, nullptr, &FrameCount, nullptr);
        FrameCount /= Width;
        if (_register) Register();
    }
    ~Animator() {
        DeRegister();
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

    void Register() {
        if (std::find(animators.begin(), animators.end(), this) == animators.end())
        animators.push_back(this);
    }

    void DeRegister() {
        animators.erase(std::remove(animators.begin(), animators.end(), this), animators.end());
    }
};

struct CopyAnimator : public Animator {
    CopyAnimator(Image * image_t, int8_t _FPS, bool Active = true, bool _register = false) {
        this->Active = Active;
        this->image = new Image{image_t->texture, image_t->Source};
        *image = *image_t;
        SDL_QueryTexture(image->texture, nullptr, nullptr, &FrameCount, nullptr);
        FrameCount /= Width;
        if (_register) animators.push_back(this);
    }

    void Copy(Image * image_t) {
        *image = *image_t;
    }

    Image* Get() {
        return image;
    }

    ~CopyAnimator() {
        if (image) delete image;
    }
};


SDL_FRect operator+(const SDL_FRect& a, const SDL_FRect& b) {
    return {a.x + b.x, a.y + b.y, a.w + b.w, a.h + b.h};
}

SDL_FRect operator+(const SDL_FRect& a, const SDL_Point& b) {
    return {a.x + b.x, a.y + b.y, a.w, a.h};
}

SDL_FRect operator*(const SDL_FRect& a, const auto& b) {
    return {a.x * b, a.y * b, a.w * b, a.h * b};
}

namespace Renderer {
    static int8_t SDL_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    static SDL_Renderer* renderer;
    static SDL_Window* window;
    static std::vector<Sprite*> sprites;
    static SDL_Event event;
    static bool running = true;
    static SDL_Color render_clear = {0, 0, 0, 0};
    void Init() {
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
        for (auto sprite : sprites) {
            DestRect = (sprite->Body * Camera::scale) + Camera::Position;
            SDL_RenderCopyF(renderer, sprite->image->texture, &sprite->image->Source, &DestRect);
        }
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }

    void Draw(const Sprite * sprite) {
        static SDL_FRect DestRect;
        DestRect = (sprite->Body * Camera::scale) + Camera::Position;
        if (SDL_RenderCopyF(renderer, sprite->image->texture, &sprite->image->Source, &DestRect) < 0) {
            std::cout << SDL_GetError() << std::endl;
        }
    }

    void Quit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void PurgeNullSprites() {
        sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [](Sprite * sprite) { return sprite == nullptr; }), sprites.end());
    }

    void SortSprites() {
        PurgeNullSprites();
        std::sort(sprites.begin(), sprites.end(), [](Sprite * a, Sprite * b) { return a->depth < b->depth; });
    }
};
