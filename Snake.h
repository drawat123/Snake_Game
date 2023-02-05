#pragma once
#include "Ball.h"
#include <SDL2/SDL_mixer.h>
#include <deque>

class Snake {
public:
  Snake(const int &sWidth, const int &sHeight, SDL_Color color);
  ~Snake();
  void Reset();
  void SetDirection(SDL_Keycode dir);
  void Render(SDL_Renderer *&gRenderer);
  bool Move(SDL_Keycode dir, Ball &b, Mix_Chunk *&gEatSound);
  int GetWidth();
  int GetHeight();
  int GetScore();

private:
  int m_Width;
  int m_Height;
  const int &SCREEN_WIDTH;
  const int &SCREEN_HEIGHT;
  const int SNAKE_VEL;
  const int INTIAL_SNAKE_LENGTH;
  SDL_Color m_Color;
  SDL_Keycode m_Dir;
  std::deque<SDL_Rect> m_RectList;
};