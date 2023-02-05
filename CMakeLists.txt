cmake_minimum_required(VERSION 3.0.0)
project(Snake_Game VERSION 0.1.0)

add_executable(Snake_Game main.cpp Snake.cpp Texture.cpp Ball.cpp)

target_link_libraries(Snake_Game SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer)