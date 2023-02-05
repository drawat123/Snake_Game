#include "Ball.h"
#include <ctime>
#include <random>
#include <vector>

std::vector<SDL_Rect> m_Grid;

Ball::Ball(const int &sWidth, const int &sHeight, SDL_Color color)
    : SCREEN_WIDTH(sWidth), SCREEN_HEIGHT(sHeight), m_Color(color) {
  Reset();

  for (int y = 0; y < SCREEN_HEIGHT / 10; y++) {
    for (int x = 0; x < SCREEN_WIDTH / 10; x++) {
      if (y == 0 && ((x * 10) == (SCREEN_WIDTH - 100) ||
                     (x * 10) == 10)) // score and high score text area.
        continue;

      m_Grid.push_back({x * 10, y * 10, 10, 10});
    }
  }
}

Ball::~Ball() {
  m_Rect = {0, 0, 0, 0};
  if (m_Timer.joinable())
    m_Timer.join();
}

void Ball::Reset() {
  int w = 10, h = 10;
  int x = ((SCREEN_WIDTH / 2) - w);
  int y = ((SCREEN_HEIGHT / 2) - h);
  m_Rect = {x, y, w, h};
}

void Ball::Render(SDL_Renderer *&gRenderer) {
  SDL_SetRenderDrawColor(gRenderer, m_Color.r, m_Color.g, m_Color.b, m_Color.a);
  SDL_RenderFillRect(gRenderer, &m_Rect);
}

SDL_Rect Ball::GetRect() { return m_Rect; }

void Ball::SetTimer() {
  m_Rect = {0, 0, 0, 0};

  if (m_Timer.joinable())
    m_Timer.join();

  m_Timer = std::thread([this]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::mt19937 rng(time(nullptr));
    std::uniform_int_distribution<int> gen(0, m_Grid.size() - 1);

    m_Rect = m_Grid[gen(rng)];
  });
}