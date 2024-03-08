#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdlib.h>

#include "inc/logger.h"
#include "inc/troid.h"

static const char texture_img_path[]     = "./assets/troidtex_8x8.png";
static SDL_Texture* texture              = NULL;
static const SDL_FPoint texture_dim      = { TROID_WIDTH, TROID_HEIGHT };
static const SDL_FPoint texture_dim_half = { TROID_WIDTH * 0.5, TROID_HEIGHT * 0.5 };

static const float one_deg_in_rad = M_PI / 180;

int troid_init(SDL_Renderer* renderer)
{
  texture = IMG_LoadTexture(renderer, texture_img_path);
  if (texture == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, IMG_GetError());
    return 1;
  }

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

  troid->velocity.x = cosf(troid->direction_r);
  troid->velocity.y = sinf(troid->direction_r);
  troid->position.x = _x;
  troid->position.y = _y;

  troid->src_rect.x = 0;
  troid->src_rect.y = 0;
  troid->src_rect.w = texture_dim.x;
  troid->src_rect.h = texture_dim.y;

  troid->dst_rect.x = _x - texture_dim_half.x;
  troid->dst_rect.y = _y - texture_dim_half.y;
  troid->dst_rect.w = texture_dim.x;
  troid->dst_rect.h = texture_dim.y;

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
  head->next = new_troid;

  return head;
}

void troid_update(struct Troid* troid, float ww, float wh)
{
  struct Troid* crnt_troid = troid;
  while (crnt_troid != NULL)
  {
    crnt_troid->position.x += crnt_troid->velocity.x;
    crnt_troid->position.y += crnt_troid->velocity.y;

    if (crnt_troid->position.x - texture_dim_half.x < 0)
    {
      crnt_troid->position.x = ww - texture_dim_half.x;
    }
    else if (crnt_troid->position.x + texture_dim_half.x > ww)
    {
      crnt_troid->position.x = texture_dim_half.x;
    }

    if (crnt_troid->position.y - texture_dim_half.y < 0)
    {
      crnt_troid->position.y = wh - texture_dim_half.y;
    }
    else if (crnt_troid->position.y + texture_dim_half.y > wh)
    {
      crnt_troid->position.y = texture_dim_half.y;
    }

    crnt_troid->dst_rect.x = crnt_troid->position.x;
    crnt_troid->dst_rect.y = crnt_troid->position.y;

    crnt_troid = crnt_troid->next;
  }
}

void troid_render(struct Troid* troid, SDL_Renderer* renderer)
{
  struct Troid* crnt_troid = troid;
  while (crnt_troid != NULL)
  {
    SDL_RenderCopyExF(renderer, texture, &crnt_troid->src_rect, &crnt_troid->dst_rect, troid->direction_d, &texture_dim_half, SDL_FLIP_NONE);
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
    free(crnt_troid);
    crnt_troid = next_troid;
  }
}

void troid_deinit(void)
{
  SDL_DestroyTexture(texture);
}
