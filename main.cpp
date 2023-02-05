#include "Snake.h"
#include "Texture.h"
#include <SDL2/SDL_mixer.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>

bool init(SDL_Window *&gWindow, SDL_Renderer *&gRenderer,
          SDL_Color &screenColor, const int &SCREEN_WIDTH,
          const int &SCREEN_HEIGHT) {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
      printf("Warning: Linear texture filtering not enabled!");

    gWindow = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      gRenderer = SDL_CreateRenderer(
          gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else
        SDL_SetRenderDrawColor(gRenderer, screenColor.r, screenColor.g,
                               screenColor.b, screenColor.a);

      int imgFlags = IMG_INIT_PNG;
      if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
               IMG_GetError());
        success = false;
      }

      if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
               TTF_GetError());
        success = false;
      }

      if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
               Mix_GetError());
        success = false;
      }
    }
  }
  return success;
}

void close(SDL_Window *&gWindow, SDL_Renderer *&gRenderer, TTF_Font *&gFont,
           Mix_Chunk *&gEatSound, Mix_Chunk *&gGameOver) {
  TTF_CloseFont(gFont);
  gFont = NULL;

  Mix_FreeChunk(gEatSound);
  Mix_FreeChunk(gGameOver);
  gEatSound = NULL;
  gGameOver = NULL;

  SDL_DestroyWindow(gWindow);
  SDL_DestroyRenderer(gRenderer);
  gWindow = NULL;
  gRenderer = NULL;

  TTF_Quit();
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}

void SetHighCore(int score) {
  std::ofstream highScoreFile("../res/Highscore.txt");
  if (highScoreFile) {
    highScoreFile << score;
    highScoreFile.close();
  }
}

int main() {
  SDL_Color snakeColor = {0, 255, 0, 255}, ballColor = {0, 0, 255, 255},
            textColor = {255, 255, 255, 255}, screenColor = {0, 0, 0, 255};
  std::string gGameOverFile = "../res/Game_Over_Dark.jpg";
  if (0) { // light mode
    textColor = {0, 0, 0, 255};
    screenColor = {255, 255, 255, 255};
    gGameOverFile = "../res/Game_Over.jpg";
  }

  std::ifstream highScoreFile("../res/Highscore.txt");
  int currHighScore = 0, currScore = 0;
  if (highScoreFile) {
    std::string line;
    highScoreFile >> currHighScore;
    highScoreFile.close();
  }

  enum class GameState { RUNNING, OVER, DELAY, QUIT };
  GameState gState = GameState::RUNNING;
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;
  SDL_Window *gWindow = NULL;
  SDL_Renderer *gRenderer = NULL;
  TTF_Font *gFont = NULL;
  LTexture gGameOverTexture;
  Snake snakeObj(SCREEN_WIDTH, SCREEN_HEIGHT, snakeColor);
  Ball ballObj(SCREEN_WIDTH, SCREEN_HEIGHT, ballColor);
  SDL_Event e;
  SDL_Keycode dir = SDLK_RIGHT;
  Mix_Chunk *gEatSound = NULL, *gGameOver = NULL;

  if (!init(gWindow, gRenderer, screenColor, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    printf("Failed to initialize!\n");
  } else {
    if (!(gGameOverTexture.loadFromFile(gRenderer, gGameOverFile)) ||
        !(gFont = TTF_OpenFont("../res/OpenSans-Regular.ttf", 20)) ||
        !(gEatSound = Mix_LoadWAV("../res/eat.wav")) ||
        !(gGameOver = Mix_LoadWAV("../res/game_over.wav"))) {
      printf("Failed to load media!\n");
    } else {
      while (gState != GameState::QUIT) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
            gState = GameState::QUIT;
            if (currScore > currHighScore)
              SetHighCore(currScore);
          } else if (gState == GameState::OVER &&
                     e.key.keysym.sym == SDLK_SPACE) {
            gState = GameState::RUNNING;
            dir = SDLK_RIGHT;
            snakeObj.Reset();
            ballObj.Reset();
          } else if (e.type == SDL_KEYDOWN) {
            if ((e.key.keysym.sym == SDLK_UP) ||
                (e.key.keysym.sym == SDLK_DOWN) ||
                (e.key.keysym.sym == SDLK_LEFT) ||
                (e.key.keysym.sym == SDLK_RIGHT))
              dir = e.key.keysym.sym;
          }
        }

        if (gState == GameState::RUNNING) {
          if (snakeObj.Move(dir, ballObj, gEatSound)) {
            currScore = snakeObj.GetScore();
            std::string highScore =
                "High Score: " + std::to_string(currScore > currHighScore
                                                    ? currScore
                                                    : currHighScore);
            std::string score = "Score: " + std::to_string(currScore);

            std::string title{highScore + "          " + score};
            SDL_SetWindowTitle(gWindow, title.c_str());
          } else
            gState = GameState::DELAY;
        }

        SDL_SetRenderDrawColor(gRenderer, screenColor.r, screenColor.g,
                               screenColor.b, screenColor.a);
        SDL_RenderClear(gRenderer);

        if (gState == GameState::RUNNING || gState == GameState::DELAY) {
          snakeObj.Render(gRenderer);
          ballObj.Render(gRenderer);
        } else if (gState == GameState::OVER) {
          SDL_Rect r = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
          gGameOverTexture.render(gRenderer, 0, 0, &r);
        }

        SDL_RenderPresent(gRenderer);

        if (gState == GameState::DELAY) {
          Mix_PlayChannel(-1, gGameOver, 0);
          gState = GameState::OVER;
          if (currScore > currHighScore) {
            currHighScore = currScore;
            SetHighCore(currHighScore);
          }

          SDL_Delay(1000);
        }

        SDL_Delay(100 -
                  ((currScore / 20) * 20)); // increasing speed of snake after
                                            // score gets increased by 20.
      }
    }
  }

  close(gWindow, gRenderer, gFont, gEatSound, gGameOver);

  return 0;
}