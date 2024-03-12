#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdbool.h>

bool gm_is_inrange_cp(SDL_FPoint* center, int radius, SDL_FPoint* point);
bool gm_is_intersect_rc(SDL_FRect* rect, SDL_FPoint* center, int radius);
