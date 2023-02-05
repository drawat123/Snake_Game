#include "Snake.h"

Snake::Snake(const int &sWidth, const int &sHeight, SDL_Color color)
    : SCREEN_WIDTH(sWidth), SCREEN_HEIGHT(sHeight), SNAKE_VEL(10),
      INTIAL_SNAKE_LENGTH(4), m_Color(color) {
  Reset();
}

Snake::~Snake() {
  m_Width = 0;
  m_Height = 0;
}

void Snake::Reset() {
  m_Width = 10;
  m_Height = 10;
  m_Dir = SDLK_RIGHT;
  m_RectList.clear();
  for (int i = 0; i < INTIAL_SNAKE_LENGTH; i++)
    m_RectList.push_back({(INTIAL_SNAKE_LENGTH - i) * SNAKE_VEL, SNAKE_VEL * 4,
                          m_Width, m_Height});
}

int Snake::GetWidth() { return m_Width; }
int Snake::GetHeight() { return m_Height; }

void Snake::Render(SDL_Renderer *&gRenderer) {
  SDL_SetRenderDrawColor(gRenderer, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
  for (int i = 1; i < m_RectList.size(); i++)
    SDL_RenderFillRect(gRenderer, &m_RectList[i]);

  SDL_SetRenderDrawColor(gRenderer, m_Color.r + 0xFF, m_Color.g - 0xFF,
                         m_Color.b, m_Color.a);
  SDL_RenderFillRect(gRenderer, &m_RectList.front());
}

void Snake::SetDirection(SDL_Keycode dir) {
  if ((m_Dir == SDLK_UP || m_Dir == SDLK_DOWN) &&
      (dir == SDLK_LEFT || dir == SDLK_RIGHT))
    m_Dir = dir;
  else if ((m_Dir == SDLK_LEFT || m_Dir == SDLK_RIGHT) &&
           (dir == SDLK_UP || dir == SDLK_DOWN))
    m_Dir = dir;
}

bool Snake::Move(SDL_Keycode dir, Ball &b, Mix_Chunk *&gEatSound) {
  int lastX = m_RectList.back().x, lastY = m_RectList.back().y;
  m_RectList.push_front(m_RectList.front());
  m_RectList.pop_back();

  SetDirection(dir);

  int velX = 0, velY = 0;
  if (m_Dir == SDLK_UP)
    velY -= SNAKE_VEL;
  if (m_Dir == SDLK_DOWN)
    velY += SNAKE_VEL;
  if (m_Dir == SDLK_LEFT)
    velX -= SNAKE_VEL;
  if (m_Dir == SDLK_RIGHT)
    velX += SNAKE_VEL;

  m_RectList.front().x += velX;
  if ((m_RectList.front().x < 0) ||
      (m_RectList.front().x + m_Width > SCREEN_WIDTH)) {
    if (m_RectList.front().x < 0)
      m_RectList.front().x = SCREEN_WIDTH - m_Width;
    if (m_RectList.front().x + m_Width > SCREEN_WIDTH)
      m_RectList.front().x = 0;
  }

  m_RectList.front().y += velY;
  if ((m_RectList.front().y < 0) ||
      (m_RectList.front().y + m_Height > SCREEN_HEIGHT)) {
    if (m_RectList.front().y < 0)
      m_RectList.front().y = SCREEN_HEIGHT - m_Height;
    if (m_RectList.front().y + m_Height > SCREEN_HEIGHT)
      m_RectList.front().y = 0;
  }

  for (int i = 1; i < m_RectList.size(); i++)
    if (SDL_HasIntersection(&m_RectList.front(), &m_RectList[i]))
      return false;

  SDL_Rect r = b.GetRect();
  if (SDL_HasIntersection(&m_RectList.front(), &r)) {
    m_RectList.push_back({lastX, lastY, m_Width, m_Height});
    Mix_PlayChannel(-1, gEatSound, 0);
    b.SetTimer();
  }

  return true;
}

int Snake::GetScore() { return m_RectList.size() - INTIAL_SNAKE_LENGTH; }