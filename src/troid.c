#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>

#include "inc/logger.h"
#include "inc/troid.h"

static const char texture_img_path[] = "./assets/troidtex_8x8.png";
static SDL_Texture* texture          = NULL;
static const float texture_w         = TROID_WIDTH;
static const float texture_h         = TROID_HEIGHT;
static const float texture_w_half    = texture_w * 0.5;
static const float texture_h_half    = texture_h * 0.5;

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

  troid->pos.x = _x;
  troid->pos.y = _y;

  troid->src_rect.x = 0;
  troid->src_rect.y = 0;
  troid->src_rect.w = texture_w;
  troid->src_rect.h = texture_h;

  troid->dst_rect.x = _x - texture_w_half;
  troid->dst_rect.y = _y - texture_h_half;
  troid->dst_rect.w = texture_w;
  troid->dst_rect.h = texture_h;

  troid->next = NULL;

  return troid;
}

struct Troid* troid_append(struct Troid* head, struct Troid* new_troid)
{
  if (new_troid == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "recived NULL for append");
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

void troid_render(struct Troid* troid, SDL_Renderer* renderer)
{
  struct Troid* crnt_troid = troid;
  while (crnt_troid != NULL)
  {
    SDL_RenderCopyF(renderer, texture, &crnt_troid->src_rect, &crnt_troid->dst_rect);
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
