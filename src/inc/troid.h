#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

struct Troid
{
  SDL_FPoint pos;
  SDL_Rect src_rect;
  SDL_FRect dst_rect;
  struct Troid* next;
};

int troid_init(SDL_Renderer* renderer);
struct Troid* troid_new(float _x, float _y);
void troid_free(struct Troid* troid);
void troid_deinit(void);
