#include <SDL2/SDL.h>
#include <iostream>
#include "gamedata.hpp"
#include "manager.hpp"
#include "content.hpp"
#include "tilemap.hpp"

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);  
  Renderer::Init();
  LoadContent();
  Tiles::TilesIndex tile;
  tile[0] = Tiles::Tile::MakeTileTemplate("dl", new Image{TextureManager::Get("Tiles/dl.bmp"), {0, 0, 16, 16}});
  tile[1] = Tiles::Tile::MakeTileTemplate("ld", new Image{TextureManager::Get("Tiles/ld.bmp"), {0, 0, 16, 16}});
  Tiles::TileMap testMap(tile, 8, 8);
  testMap.LoadTileMap({
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1
  });
  Image right = {TextureManager::Get("right.png"), {0, 0, 16, 16}};
  Image front = {TextureManager::Get("front.png"), {0, 0, 16, 16}};
  Image back = {TextureManager::Get("back.png"), {0, 0, 16, 16}};
  Image left = {TextureManager::Get("left.png"), {0, 0, 16, 16}};
  Image white = {TextureManager::Get("white.png"), {0, 0, 16, 16}};
  Animator back_anim(&back, 5);
  Animator front_anim(&front, 5);
  Animator left_anim(&left, 5);
  Animator right_anim(&right, 5);
  SpriteManager::Arena arena;
  Sprite * testRun = arena.New(SpriteKind::Player, SDL_FRect{0.0f, 0.0f, 32.0f, 32.0f}, &front);
  Sprite * testRun2 = arena.New(SpriteKind::Player, SDL_FRect{10.0f, 10.0f, 32.0f, 32.0f}, &white);
  arena.SwapRenderList();
  Renderer::SortSprites();
  Camera::Position = {Camera::width / 2, Camera::height / 2};
  while (Renderer::running) {
    TimeManager::AdvanceTime();
    Animator::GlobalUpdate();
    Renderer::Input();
    Renderer::Update();
    testMap.Draw();
    Renderer::Render();
    InputManager::CopyCurrentState();
    if (InputManager::keyboard_state[SDL_SCANCODE_UP]) Camera::Position.y += TimeManager::DeltaTime;
    if (InputManager::keyboard_state[SDL_SCANCODE_DOWN]) Camera::Position.y -= TimeManager::DeltaTime;
    if (InputManager::keyboard_state[SDL_SCANCODE_RIGHT]) Camera::Position.x -= TimeManager::DeltaTime;
    if (InputManager::keyboard_state[SDL_SCANCODE_LEFT]) Camera::Position.x += TimeManager::DeltaTime;
  }
  TextureManager::Clear();
  Renderer::Quit();
}