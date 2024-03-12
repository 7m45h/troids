#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>

#include "inc/geom.h"

bool gm_is_inrange_cp(SDL_FPoint* center, int radius, SDL_FPoint* point)
{
  float delta_x = center->x - point->x;
  float delta_y = center->y - point->y;

  if ((delta_x * delta_x + delta_y * delta_y) < radius * radius)
  {
    return true;
  }

  return false;
}

bool gm_is_intersect_rc(SDL_FRect* rect, SDL_FPoint* center, int radius)
{
  SDL_FPoint closest = { rect->x + rect->w, rect->y + rect->h };

  if (center->x < rect->x)
  {
    closest.x = rect->x;
  }
  else if (center->x < closest.x)
  {
    closest.x = center->x;
  }

  if (center->y < rect->y)
  {
    closest.y = rect->y;
  }
  else if (center->y < closest.y)
  {
    closest.y = center->y;
  }

  return gm_is_inrange_cp(center, radius, &closest);
}
