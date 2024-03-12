#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>

#include "quadtree.h"

#define WORLD_PADDING        128
#define WORLD_PADDING_DOUBLE 256

struct World
{
  SDL_FRect   window_dim;
  SDL_FRect    world_dim;
  SDL_Window*     window;
  SDL_Renderer* renderer;
  SDL_Event        event;
  bool          evolving;

  struct Quadtree*    qt;
};

struct World* world_form(const char* title, float w, float h);
void world_evolve(struct World* world);
void world_free(struct World* world);
