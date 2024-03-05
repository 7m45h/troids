#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>

#include "quadtree.h"

struct World
{
  float window_w;
  float window_h;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Event event;
  bool evolving;

  struct Quadtree* qt;
};

struct World* world_form(const char* title, float w, float h);
void world_evolve(struct World* world);
void world_free(struct World* world);
