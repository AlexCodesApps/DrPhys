#include <SDL2/SDL.h>
#include <iostream>
#include "gamedata.hpp"
#include "manager.hpp"

int main() {
  Renderer::Init();
  TextureManager::BatchLoad({"back.png","left.png","right.png", "front.png", "white.png"});
  Image right = {TextureManager::Get("right.png"), {0, 0, 16, 16}};
  Image front = {TextureManager::Get("front.png"), {0, 0, 16, 16}};
  Image back = {TextureManager::Get("back.png"), {0, 0, 16, 16}};
  Image left = {TextureManager::Get("left.png"), {0, 0, 16, 16}};
  Image white = {TextureManager::Get("white.png"), {0, 0, 16, 16}};
  Animator back_anim(&back, 10);
  Animator front_anim(&front, 10);
  Animator left_anim(&left, 10);
  Animator right_anim(&right, 10);
  SpriteManager::Arena arena;
  Sprite * testRun = arena.New(SpriteKind::Player, SDL_FRect{0.0f, 0.0f, 32.0f, 32.0f}, &front);
  Sprite * testRun2 = arena.New(SpriteKind::Player, SDL_FRect{64.0f, 64.0f, 32.0f, 32.0f}, &white);
  arena.SwapRenderList();
  Renderer::SortSprites();
  Camera::Position = {Camera::width / 2, Camera::height / 2};
  while (Renderer::running) {
    TimeManager::AdvanceTime();
    Animator::GlobalUpdate();
    Renderer::Input();
    Renderer::Update();
    Renderer::Render();
    InputManager::CopyCurrentState();
    if (InputManager::keyboard_state[SDL_SCANCODE_UP]) testRun->image = &back;
    if (InputManager::keyboard_state[SDL_SCANCODE_DOWN]) testRun->image = &front;
    if (InputManager::keyboard_state[SDL_SCANCODE_RIGHT]) testRun->image = &right;
    if (InputManager::keyboard_state[SDL_SCANCODE_LEFT]) testRun->image = &left;
  }
  TextureManager::Clear();
  Renderer::Quit();
}