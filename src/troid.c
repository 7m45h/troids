#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdlib.h>

#include "inc/darray.h"
#include "inc/geom.h"
#include "inc/logger.h"
#include "inc/troid.h"

static const char texture_img_path[]     = "./assets/troidtex_8x8.png";
static SDL_Texture* texture              = NULL;
static const SDL_FPoint texture_dim      = { TROID_WIDTH, TROID_HEIGHT };
static const SDL_FPoint texture_dim_half = { TROID_WIDTH * 0.5, TROID_HEIGHT * 0.5 };

// static const char scan_range_texture_img_path[]   = "./assets/troid_scan_rangetex_64x64.png";
// static SDL_Texture* scan_range_texture            = NULL;
// static const int scan_range_texture_dim           = TROID_PERCEPTION_RADIUS * 2;
// static const int scan_range_texture_dim_half      = TROID_PERCEPTION_RADIUS;
// static const SDL_Rect scan_range_texture_src_rect = {0, 0, 64, 64};

static struct Troid* neighbor            = NULL;
static SDL_FPoint neighbors_avg_velocity = {0, 0};
static SDL_FPoint neighbors_avg_position = {0, 0};

static const float one_deg_in_rad = M_PI / 180;
static const float one_rad_in_deg = 180 / M_PI;

static const float separation_factor = 0.05;
static const float alignment_factor  = 0.05;
static const float cohesion_factor   = 0.0005;
static const float turn_factor       = 0.5;

static SDL_FRect swim_area = {0, 0, 0, 0};

void troid_set_swim_area(SDL_FRect* window_dim)
{
  swim_area.x = window_dim->x + TROID_PERCEPTION_RADIUS;
  swim_area.y = window_dim->y + TROID_PERCEPTION_RADIUS;
  swim_area.w = window_dim->w - TROID_PERCEPTION_RADIUS;
  swim_area.h = window_dim->h - TROID_PERCEPTION_RADIUS;
}

int troid_init(SDL_Renderer* renderer)
{
  texture = IMG_LoadTexture(renderer, texture_img_path);
  if (texture == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, IMG_GetError());
    return 1;
  }

  // scan_range_texture = IMG_LoadTexture(renderer, scan_range_texture_img_path);
  // if (scan_range_texture == NULL)
  // {
  //   logger(ERROR, __FILE_NAME__, __LINE__, IMG_GetError());
  //   return 1;
  // }

  return 0;
}

struct Troid* troid_new(float _x, float _y)
{
  struct Troid* troid = malloc(sizeof(struct Troid));
  if (troid == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "malloc sizeof struct Troid returned NULL");
    return NULL;
  }

  troid->direction_d = rand() % 360;
  troid->direction_r = troid->direction_d * one_deg_in_rad;

  troid->acceleration.x = 0;
  troid->acceleration.y = 0;
  troid->velocity.x     = cosf(troid->direction_r);
  troid->velocity.y     = sinf(troid->direction_r);
  troid->position.x     = _x;
  troid->position.y     = _y;

  troid->src_rect.x = 0;
  troid->src_rect.y = 0;
  troid->src_rect.w = texture_dim.x;
  troid->src_rect.h = texture_dim.y;

  troid->dst_rect.x = _x - texture_dim_half.x;
  troid->dst_rect.y = _y - texture_dim_half.y;
  troid->dst_rect.w = texture_dim.x;
  troid->dst_rect.h = texture_dim.y;

  // troid->scan_range_dst_rect.x = _x - scan_range_texture_dim_half;
  // troid->scan_range_dst_rect.y = _y - scan_range_texture_dim_half;
  // troid->scan_range_dst_rect.w = scan_range_texture_dim;
  // troid->scan_range_dst_rect.h = scan_range_texture_dim;

  troid->neighbors = da_new(DEFAULT_DARRAY_CAP);
  if (troid->neighbors == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "create new darray failed");
    free(troid);
    return NULL;
  }

  troid->next = NULL;

  return troid;
}

struct Troid* troid_append(struct Troid* head, struct Troid* new_troid)
{
  if (new_troid == NULL)
  {
    // logger(ERROR, __FILE_NAME__, __LINE__, "recived NULL for append");
    return head;
  }

  if (head == NULL)
  {
    return new_troid;
  }

  struct Troid* crnt_troid = new_troid;
  struct Troid* next_troid = crnt_troid->next;
  while (next_troid != NULL)
  {
    crnt_troid = next_troid;
    next_troid = crnt_troid->next;
  }

  crnt_troid->next = head->next;
  head->next       = new_troid;

  return head;
}

void troid_update(struct Troid* troid)
{
  struct Troid* crnt_troid = troid;
  while (crnt_troid != NULL)
  {
    for (int i = 0; i < crnt_troid->neighbors->len; i++)
    {
      if (crnt_troid->neighbors->itmes[i] == crnt_troid)
      {
        da_remove(crnt_troid->neighbors, i);
        break;
      }
    }

    neighbors_avg_position.x   = 0;
    neighbors_avg_position.y   = 0;
    neighbors_avg_velocity.x   = 0;
    neighbors_avg_velocity.y   = 0;
    crnt_troid->acceleration.x = 0;
    crnt_troid->acceleration.y = 0;

    for (int i = 0; i < crnt_troid->neighbors->len; i++)
    {
      neighbor = crnt_troid->neighbors->itmes[i];
      if (gm_is_inrange_cp(&crnt_troid->position, TROID_PRIVATE_RADIUS, &neighbor->position))
      {
        crnt_troid->acceleration.x += (crnt_troid->position.x - neighbor->position.x) * separation_factor;
        crnt_troid->acceleration.y += (crnt_troid->position.y - neighbor->position.y) * separation_factor;
      }
      neighbors_avg_velocity.x += neighbor->velocity.x;
      neighbors_avg_velocity.y += neighbor->velocity.y;
      neighbors_avg_position.x += neighbor->position.x;
      neighbors_avg_position.y += neighbor->position.y;
    }

    if (crnt_troid->neighbors->len > 0)
    {
      neighbors_avg_velocity.x = neighbors_avg_velocity.x / crnt_troid->neighbors->len;
      neighbors_avg_velocity.y = neighbors_avg_velocity.y / crnt_troid->neighbors->len;
      neighbors_avg_position.x = neighbors_avg_position.x / crnt_troid->neighbors->len;
      neighbors_avg_position.y = neighbors_avg_position.y / crnt_troid->neighbors->len;

      crnt_troid->acceleration.x += (neighbors_avg_velocity.x - crnt_troid->velocity.x) * alignment_factor;
      crnt_troid->acceleration.y += (neighbors_avg_velocity.y - crnt_troid->velocity.y) * alignment_factor;
      crnt_troid->acceleration.x += (neighbors_avg_position.x - crnt_troid->position.x) * cohesion_factor;
      crnt_troid->acceleration.y += (neighbors_avg_position.y - crnt_troid->position.y) * cohesion_factor;
    }

    crnt_troid->velocity.x += crnt_troid->acceleration.x;
    crnt_troid->velocity.y += crnt_troid->acceleration.y;
    crnt_troid->position.x += crnt_troid->velocity.x;
    crnt_troid->position.y += crnt_troid->velocity.y;

    if (crnt_troid->position.x < swim_area.x)
    {
      crnt_troid->velocity.x += turn_factor;
    }
    else if (crnt_troid->position.x > swim_area.w)
    {
      crnt_troid->velocity.x -= turn_factor;
    }

    if (crnt_troid->position.y < swim_area.y)
    {
      crnt_troid->velocity.y += turn_factor;
    }
    else if (crnt_troid->position.y > swim_area.h)
    {
      crnt_troid->velocity.y -= turn_factor;
    }

    crnt_troid->direction_r = atan2(crnt_troid->velocity.y, crnt_troid->velocity.x);
    crnt_troid->direction_d = crnt_troid->direction_r * one_rad_in_deg;

    crnt_troid->dst_rect.x = crnt_troid->position.x - texture_dim_half.x;
    crnt_troid->dst_rect.y = crnt_troid->position.y - texture_dim_half.y;

    // crnt_troid->scan_range_dst_rect.x = crnt_troid->position.x - scan_range_texture_dim_half;
    // crnt_troid->scan_range_dst_rect.y = crnt_troid->position.y - scan_range_texture_dim_half;

    crnt_troid = crnt_troid->next;
  }
}

void troid_render(struct Troid* troid, SDL_Renderer* renderer)
{
  struct Troid* crnt_troid = troid;
  while (crnt_troid != NULL)
  {
    SDL_RenderCopyExF(renderer, texture, &crnt_troid->src_rect, &crnt_troid->dst_rect, troid->direction_d, &texture_dim_half, SDL_FLIP_NONE);
    // SDL_RenderCopyF(renderer, scan_range_texture, &scan_range_texture_src_rect, &crnt_troid->scan_range_dst_rect);
    // for (int i = 0; i < crnt_troid->neighbors->len; i++)
    // {
    //  neighbor = crnt_troid->neighbors->itmes[i];
    //  SDL_RenderDrawRectF(renderer, &neighbor->dst_rect);
    // }
    crnt_troid = crnt_troid->next;
  }
}

void troid_free(struct Troid* troid)
{
  struct Troid* crnt_troid = troid;
  struct Troid* next_troid = NULL;

  while (crnt_troid != NULL)
  {
    next_troid = crnt_troid->next;
    da_free(crnt_troid->neighbors);
    free(crnt_troid);
    crnt_troid = next_troid;
  }
}

void troid_deinit(void)
{
  // SDL_DestroyTexture(scan_range_texture);
  SDL_DestroyTexture(texture);
}
