#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/logger.h"
#include "inc/quadtree.h"
#include "inc/troid.h"

static const int quad_safe_offset        = 5;
static const int quad_safe_offset_double = quad_safe_offset * 2;

struct Quadtree* qt_new(float _x, float _y, float _w, float _h)
{
  struct Quadtree* qt = malloc(sizeof(struct Quadtree));
  if (qt == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "malloc sizeof struct Quadtree returned NULL");
    return NULL;
  }

  qt->real_dim.x = _x;
  qt->real_dim.y = _y;
  qt->real_dim.w = _w;
  qt->real_dim.h = _h;

  qt->safe_dim.x = _x - quad_safe_offset;
  qt->safe_dim.y = _y - quad_safe_offset;
  qt->safe_dim.w = _w + quad_safe_offset_double;
  qt->safe_dim.h = _h + quad_safe_offset_double;

  qt->len    = 0;
  qt->troids = NULL;

  qt->divided = false;
  qt->nw      = NULL;
  qt->ne      = NULL;
  qt->sw      = NULL;
  qt->se      = NULL;

  return qt;
}

void qt_render(struct Quadtree* qt, SDL_Renderer* renderer)
{
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawRectF(renderer, &qt->real_dim);
  SDL_RenderDrawRectF(renderer, &qt->safe_dim);
}

void qt_free(struct Quadtree* qt)
{
  if (qt->divided)
  {
    qt_free(qt->nw);
    qt_free(qt->ne);
    qt_free(qt->sw);
    qt_free(qt->se);
  }

  troid_free(qt->troids);
  free(qt);
}
