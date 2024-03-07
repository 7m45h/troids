#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/logger.h"
#include "inc/quadtree.h"
#include "inc/troid.h"

static const int quad_safe_offset        = 5;
static const int quad_safe_offset_double = quad_safe_offset * 2;

// static const int quad_min_w = TROID_WIDTH * 4;
// static const int quad_min_h = TROID_HEIGHT * 4;

static bool qt_add_to_branch(struct Quadtree* qt, struct Troid* troid)
{
  if (qt_add(qt->nw, troid)) return true;
  if (qt_add(qt->ne, troid)) return true;
  if (qt_add(qt->sw, troid)) return true;
  if (qt_add(qt->se, troid)) return true;
  return false;
}

static bool qt_divid(struct Quadtree* qt)
{
  float half_w = qt->real_dim.w * 0.5;
  float half_h = qt->real_dim.h * 0.5;

  qt->nw = qt_new(qt->real_dim.x,          qt->real_dim.y,          half_w, half_h);
  qt->ne = qt_new(qt->real_dim.x + half_w, qt->real_dim.y,          half_w, half_h);
  qt->sw = qt_new(qt->real_dim.x,          qt->real_dim.y + half_h, half_w, half_h);
  qt->se = qt_new(qt->real_dim.x + half_w, qt->real_dim.y + half_h, half_w, half_h);

  if (qt->nw == NULL || qt->ne == NULL || qt->sw == NULL || qt->se == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "qt_divid failed");
    qt_free(qt->nw);
    qt_free(qt->ne);
    qt_free(qt->sw);
    qt_free(qt->se);
    return false;
  }

  struct Troid* crnt_troid = qt->troids;
  struct Troid* next_troid = NULL;
  while (crnt_troid != NULL)
  {
    next_troid = crnt_troid->next;
    crnt_troid->next = NULL;
    if (!qt_add_to_branch(qt, crnt_troid))
    {
      logger(ERROR, __FILE_NAME__, __LINE__, "troid not accepted by any branch");
      troid_free(crnt_troid);
    }
    crnt_troid = next_troid;
  }

  qt->troids = NULL;
  return true;
}

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

  qt->troids = NULL;

  qt->divided = false;
  qt->nw      = NULL;
  qt->ne      = NULL;
  qt->sw      = NULL;
  qt->se      = NULL;

  return qt;
}

bool qt_add(struct Quadtree* qt, struct Troid* troid)
{
  if (!SDL_PointInFRect(&troid->pos, &qt->safe_dim))
  {
    return false;
  }

  if (qt->troids == NULL && !qt->divided)
  {
    qt->troids = troid_append(qt->troids, troid);
    return true;
  }

  if (!qt->divided)
  {
    qt->divided = qt_divid(qt);
  }

  if (qt->divided)
  {
    if (qt_add_to_branch(qt, troid))
    {
      return true;
    }
  }

  logger(ERROR, __FILE_NAME__, __LINE__, "troid not accepted");
  troid_free(troid);
  return true;
}

void qt_render(struct Quadtree* qt, SDL_Renderer* renderer)
{
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawRectF(renderer, &qt->real_dim);
  SDL_RenderDrawRectF(renderer, &qt->safe_dim);

  if (qt->divided)
  {
    qt_render(qt->nw, renderer);
    qt_render(qt->ne, renderer);
    qt_render(qt->sw, renderer);
    qt_render(qt->se, renderer);
  }
  else
  {
    troid_render(qt->troids, renderer);
  }
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
