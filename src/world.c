#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>

// #include "inc/darray.h"
#include "inc/logger.h"
#include "inc/quadtree.h"
#include "inc/troid.h"
#include "inc/world.h"

// static const int scanner_dim             = 256;
// static const int scanner_dim_half        = scanner_dim * 0.5;
// static SDL_FRect scanner_rect            = { 0, 0, 256, 256 };
// static struct Darray* scanner_results    = NULL;
// static struct Troid* scanner_result_item = NULL;

static bool world_init(struct World* world)
{
  world->qt = qt_new(world->world_dim.x, world->world_dim.y, world->world_dim.h, world->world_dim.w);
  if (world->qt == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "qt_new returned NULL");
    return false;
  }

  int troid_status = troid_init(world->renderer);
  if (troid_status != 0)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "troid init failed");
    return false;
  }

  // scanner_results = da_new(DEFAULT_DARRAY_CAP);
  // if (scanner_results == NULL)
  // {
  //   logger(ERROR, __FILE_NAME__, __LINE__, "darray init failed");
  //   return false;
  // }

  return true;
}

static void world_deinit(struct World* world)
{
  troid_deinit();
  if (world->qt != NULL)
  {
    qt_free(world->qt);
  }
  // if (scanner_results != NULL)
  // {
  //   da_free(scanner_results);
  // }
}

static void world_handle_events(struct World* world)
{
  while (SDL_PollEvent(&world->event))
  {
    switch (world->event.type)
    {
      // case SDL_MOUSEMOTION:
      // scanner_rect.x = world->event.motion.x - scanner_dim_half;
      // scanner_rect.y = world->event.motion.y - scanner_dim_half;
      // break;

      case SDL_MOUSEBUTTONUP:
      switch (world->event.button.button)
      {
        case SDL_BUTTON_LEFT:
        qt_add(world->qt, troid_new(world->event.button.x, world->event.button.y));
        break;
      }
      break;

      case SDL_WINDOWEVENT:
      switch (world->event.window.event)
      {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        world->window_dim.w = world->event.window.data1;
        world->window_dim.h = world->event.window.data2;
        world->world_dim.w  = world->event.window.data1 + WORLD_PADDING_DOUBLE;
        world->world_dim.h  = world->event.window.data2 + WORLD_PADDING_DOUBLE;
        qt_handle_window_resize(world->qt, &world->world_dim);
        break;
      }
      break;

      case SDL_QUIT:
      world->evolving = false;
      break;

      case SDL_KEYDOWN:
      switch (world->event.key.keysym.sym)
      {
        case SDLK_q:
        world->evolving = false;
        break;
      }
      break;
    }
  }
}

static void world_render(struct World* world)
{
  SDL_SetRenderDrawColor(world->renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(world->renderer);

  SDL_SetRenderDrawColor(world->renderer, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  qt_render(world->qt, world->renderer);

  // SDL_SetRenderDrawColor(world->renderer, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  // for (int i = 0; i < scanner_results->len; i++)
  // {
  //   scanner_result_item = (struct Troid*) scanner_results->itmes[i];
  //   SDL_RenderDrawRectF(world->renderer, &scanner_result_item->dst_rect);
  // }

  // SDL_SetRenderDrawColor(world->renderer, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE);
  // SDL_RenderDrawRectF(world->renderer, &scanner_rect);

  SDL_RenderPresent(world->renderer);
}

struct World* world_form(const char* title, float w, float h)
{
  int sdl_status = SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  if (sdl_status != 0)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, SDL_GetError());
    return NULL;
  }

  int img_status = IMG_Init(IMG_INIT_PNG);
  if (img_status == 0)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, IMG_GetError());
    SDL_Quit();
    return NULL;
  }

  struct World* world = malloc(sizeof(struct World));
  if (world == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "malloc sizeof struct World returned NULL");
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  world->window_dim.x = 0;
  world->window_dim.y = 0;
  world->window_dim.w = w;
  world->window_dim.h = h;
  world->world_dim.x  = 0 - WORLD_PADDING;
  world->world_dim.y  = 0 - WORLD_PADDING;
  world->world_dim.w  = w + WORLD_PADDING_DOUBLE;
  world->world_dim.h  = h + WORLD_PADDING_DOUBLE;

  world->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
  if (world->window == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, SDL_GetError());
    free(world);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  world->renderer = SDL_CreateRenderer(world->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (world->renderer == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, SDL_GetError());
    SDL_DestroyWindow(world->window);
    free(world);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  world->evolving = false;

  return world;
}

void world_evolve(struct World* world)
{
  world->evolving = world_init(world);

  while (world->evolving)
  {
    world_handle_events(world);
    // da_empty(scanner_results);
    qt_update(world->qt, world->qt, &world->window_dim);
    // qt_query_r(world->qt, &scanner_rect, scanner_results);
    world_render(world);
  }

  world_deinit(world);
}

void world_free(struct World* world)
{
  SDL_DestroyRenderer(world->renderer);
  SDL_DestroyWindow(world->window);
  free(world);
  IMG_Quit();
  SDL_Quit();
  logger(INFO, __FILE_NAME__, __LINE__, "quit");
}
