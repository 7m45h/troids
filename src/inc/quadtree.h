#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

#include "darray.h"
#include "troid.h"

struct Quadtree
{
  SDL_FRect   real_dim;
  SDL_FRect   safe_dim;
  struct Troid* troids;
  int              len;
  bool         divided;
  struct Quadtree*  nw;
  struct Quadtree*  ne;
  struct Quadtree*  sw;
  struct Quadtree*  se;
};

struct Quadtree* qt_new(float _x, float _y, float _w, float _h);
void qt_handle_window_resize(struct Quadtree* qt_root, SDL_FRect* world_dim);
bool qt_add(struct Quadtree* qt, struct Troid* troid);
void qt_update(struct Quadtree* qt, struct Quadtree* qt_root);
void qt_query_r(struct Quadtree* qt, SDL_FRect* range, struct Darray* results);
void qt_query_c(struct Quadtree* qt, SDL_FPoint* center, int radius, struct Darray* results);
void qt_render(struct Quadtree* qt, SDL_Renderer* renderer);
void qt_free(struct Quadtree* qt);
