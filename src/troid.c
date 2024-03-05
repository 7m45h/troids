#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

#include "inc/logger.h"
#include "inc/troid.h"

static const char texture_img_path[] = "./assets/troidtex_8x8.png";

static SDL_Texture* texture = NULL;
static const float texture_w = 8;
static const float texture_h = 8;
static const float texture_w_half = texture_w * 0.5;
static const float texture_h_half = texture_h * 0.5;

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

void troid_deinit(void)
{
  SDL_DestroyTexture(texture);
}
