#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdbool.h>

struct Quadtree
{
  SDL_FRect real_dim;
  SDL_FRect safe_dim;
  int len;
  bool divided;
  struct Quadtree* nw;
  struct Quadtree* ne;
  struct Quadtree* sw;
  struct Quadtree* se;
};
