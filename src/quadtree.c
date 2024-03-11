#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/darray.h"
#include "inc/logger.h"
#include "inc/quadtree.h"
#include "inc/troid.h"

static const int quad_safe_offset        = 5;
static const int quad_safe_offset_double = quad_safe_offset * 2;

static const int quad_min_w   = TROID_WIDTH * 4;
static const int quad_min_h   = TROID_HEIGHT * 4;
static const int quad_max_len = 1;

static void qt_set_quad_size(struct Quadtree* qt, float _x, float _y, float _w, float _h)
{
  qt->real_dim.x = _x;
  qt->real_dim.y = _y;
  qt->real_dim.w = _w;
  qt->real_dim.h = _h;

  qt->safe_dim.x = _x - quad_safe_offset;
  qt->safe_dim.y = _y - quad_safe_offset;
  qt->safe_dim.w = _w + quad_safe_offset_double;
  qt->safe_dim.h = _h + quad_safe_offset_double;
}

static void qt_resize(struct Quadtree* qt, float _x, float _y, float _w, float _h)
{
  qt_set_quad_size(qt, _x, _y, _w, _h);
  if (qt->divided)
  {
    float half_w = qt->real_dim.w * 0.5;
    float half_h = qt->real_dim.h * 0.5;

    qt_resize(qt->nw, qt->real_dim.x,          qt->real_dim.y,          half_w, half_h);
    qt_resize(qt->ne, qt->real_dim.x + half_w, qt->real_dim.y,          half_w, half_h);
    qt_resize(qt->sw, qt->real_dim.x,          qt->real_dim.y + half_h, half_w, half_h);
    qt_resize(qt->se, qt->real_dim.x + half_w, qt->real_dim.y + half_h, half_w, half_h);
  }
}

static bool qt_add_to_branch(struct Quadtree* qt, struct Troid* troid)
{
  if (qt_add(qt->nw, troid)) return true;
  if (qt_add(qt->ne, troid)) return true;
  if (qt_add(qt->sw, troid)) return true;
  if (qt_add(qt->se, troid)) return true;
  return false;
}

static void qt_rearrange(struct Quadtree* qt, struct Quadtree* qt_root)
{
  struct Troid* crnt_troid = qt->troids;
  struct Troid* next_troid = NULL;
  qt->troids = NULL;
  qt->len    = 0;

  while (crnt_troid != NULL)
  {
    next_troid = crnt_troid->next;
    crnt_troid->next = NULL;

    if (SDL_PointInFRect(&crnt_troid->position, &qt->safe_dim))
    {
      qt_add(qt, crnt_troid);
    }
    else
    {
      if(!qt_add(qt_root, crnt_troid))
      {
        logger(ERROR, __FILE_NAME__, __LINE__, "troid not accepted by root quad");
        troid_free(crnt_troid);
      }
    }

    crnt_troid = next_troid;
  }

  if (qt->divided)
  {
    qt_rearrange(qt->nw, qt_root);
    qt_rearrange(qt->ne, qt_root);
    qt_rearrange(qt->sw, qt_root);
    qt_rearrange(qt->se, qt_root);
  }
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
  qt->troids = NULL;
  qt->len    = 0;

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

  qt_set_quad_size(qt, _x, _y, _w, _h);

  qt->troids = NULL;
  qt->len    = 0;

  qt->divided = false;
  qt->nw      = NULL;
  qt->ne      = NULL;
  qt->sw      = NULL;
  qt->se      = NULL;

  return qt;
}

void qt_handle_window_resize(struct Quadtree* qt_root, float ww, float wh)
{
  qt_resize(qt_root, 0, 0, ww, wh);
  qt_rearrange(qt_root, qt_root);
}

bool qt_add(struct Quadtree* qt, struct Troid* troid)
{
  if (!SDL_PointInFRect(&troid->position, &qt->safe_dim))
  {
    return false;
  }

  if (!qt->divided)
  {
    if (qt->len < quad_max_len || qt->real_dim.w < quad_min_w || qt->real_dim.h < quad_min_h)
    {
      qt->troids = troid_append(qt->troids, troid);
      qt->len++;
      return true;
    }

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

void qt_update(struct Quadtree* qt, struct Quadtree* qt_root, float ww, float wh)
{
  troid_update(qt->troids, ww, wh);
  qt_rearrange(qt, qt_root);

  if (qt->divided)
  {
    qt_update(qt->nw, qt_root, ww, wh);
    qt_update(qt->ne, qt_root, ww, wh);
    qt_update(qt->sw, qt_root, ww, wh);
    qt_update(qt->se, qt_root, ww, wh);

    if (!qt->nw->divided && !qt->ne->divided && !qt->sw->divided && !qt->se->divided)
    {
      int child_troid_count = qt->nw->len + qt->ne->len + qt->sw->len + qt->se->len;
      if (child_troid_count <= quad_max_len)
      {
        if (child_troid_count > 0)
        {
          qt->troids = troid_append(qt->troids, qt->nw->troids);
          qt->troids = troid_append(qt->troids, qt->ne->troids);
          qt->troids = troid_append(qt->troids, qt->sw->troids);
          qt->troids = troid_append(qt->troids, qt->se->troids);
          qt->len    = child_troid_count;

          qt->nw->troids = NULL;
          qt->ne->troids = NULL;
          qt->sw->troids = NULL;
          qt->se->troids = NULL;
        }

        qt_free(qt->nw);
        qt_free(qt->ne);
        qt_free(qt->sw);
        qt_free(qt->se);

        qt->divided = false;
        qt->nw      = NULL;
        qt->ne      = NULL;
        qt->sw      = NULL;
        qt->se      = NULL;
      }
    }
  }
}

void qt_query(struct Quadtree* qt, SDL_FRect* range, struct Darray* results)
{
  if (SDL_HasIntersectionF(&qt->real_dim, range))
  {
    struct Troid* crnt_troid = qt->troids;
    while (crnt_troid != NULL)
    {
      if (SDL_PointInFRect(&crnt_troid->position, range))
      {
        if (!da_add_item(results, crnt_troid))
        {
          logger(ERROR, __FILE_NAME__, __LINE__, "da add new item failed");
        }
      }

      crnt_troid = crnt_troid->next;
    }

    if (qt->divided)
    {
      qt_query(qt->nw, range, results);
      qt_query(qt->ne, range, results);
      qt_query(qt->sw, range, results);
      qt_query(qt->se, range, results);
    }
  }
}

void qt_render(struct Quadtree* qt, SDL_Renderer* renderer)
{
  // SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
  // SDL_RenderDrawRectF(renderer, &qt->real_dim);
  // SDL_RenderDrawRectF(renderer, &qt->safe_dim);

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
