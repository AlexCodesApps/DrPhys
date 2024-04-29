#include <SDL2/SDL.h>
#include <iostream>
#include "gamedata.hpp"
#include "manager.hpp"
#include "content.hpp"
#include "tilemap.hpp"
#include "gamestate.hpp"

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  {
    Dimensions screen = Camera::GetScreenSize();
    Camera::width = screen.x/4;
    Camera::height = screen.y/4;
    Camera::scale = 2.0f;
  }  
  Renderer::Init();
  ContentManager::LoadContent();
  GameState::Init();
  Renderer::SortSprites();
  Camera::Position = {(float)Camera::width / 2, (float)Camera::height / 2};
  while (Renderer::running) {
    TimeManager::AdvanceTime();
    Animator::GlobalUpdate();
    Renderer::Input();
    GameState::Input();
    GameState::Update();
    Renderer::Update();
    Tiles::TileMapManager::Draw();
    Renderer::Render();
    InputManager::CopyCurrentState();
  }
  ContentManager::ClearDynamicImages();
  ContentManager::Clear();
  Renderer::Quit();
}