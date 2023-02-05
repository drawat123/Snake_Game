#pragma once
#include <SDL2/SDL.h>
#include <thread>

class Ball {
public:
  Ball(const int &sWidth, const int &sHeight, SDL_Color color);
  ~Ball();
  void Reset();
  void Render(SDL_Renderer *&gRenderer);
  SDL_Rect GetRect();
  void SetTimer();

private:
  SDL_Rect m_Rect;
  std::thread m_Timer;
  SDL_Color m_Color;
  const int &SCREEN_WIDTH;
  const int &SCREEN_HEIGHT;
};