#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#define TROID_WIDTH  8
#define TROID_HEIGHT 8

struct Troid
{
  SDL_FPoint pos;
  SDL_Rect src_rect;
  SDL_FRect dst_rect;
  struct Troid* next;
};

int troid_init(SDL_Renderer* renderer);
struct Troid* troid_new(float _x, float _y);
struct Troid* troid_append(struct Troid* head, struct Troid* new_troid);
void troid_render(struct Troid* troid, SDL_Renderer* renderer);
void troid_free(struct Troid* troid);
void troid_deinit(void);
