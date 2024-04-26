#include <SDL2/SDL.h>
#include <iostream>
#include "gamedata.hpp"

int main() {
  Renderer::Init();
  AnimatedSprite testSpr;
  testSpr.frameCount = 2;
  testSpr.base_sprite = {SpriteKind::Player, 0, IMG_LoadTexture(Renderer::renderer, "../Assets/front.png"), {0, 0, 16, 16}, {0, 0, 64, 64}};
  Renderer::RenderList.push_back(&testSpr.base_sprite);
  Camera::Position = {Camera::width / 2, Camera::height / 2};
  while (Renderer::running) {
    AnimatedSprite::GlobalUpdate();
    Renderer::Input();
    Renderer::Update();
    Renderer::Render();
  }
  Renderer::Quit();
}