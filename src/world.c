#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/logger.h"
#include "inc/troid.h"
#include "inc/world.h"

static void world_handle_events(struct World* world)
{
  while (SDL_PollEvent(&world->event))
  {
    switch (world->event.type)
    {
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

      case SDL_WINDOWEVENT:
      switch (world->event.window.event)
      {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        world->window_w = world->event.window.data1;
        world->window_h = world->event.window.data2;
        break;
      }
      break;
    }
  }
}

static void world_render(struct World* world)
{
  SDL_SetRenderDrawColor(world->renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(world->renderer);

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

  world->window_w = w;
  world->window_h = h;

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

  int troid_status = troid_init(world->renderer);
  if (troid_status != 0)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "troid init failed");
    SDL_DestroyRenderer(world->renderer);
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
  world->evolving = true;

  while (world->evolving)
  {
    world_handle_events(world);
    world_render(world);
  }
}

void world_free(struct World* world)
{
  troid_deinit();

  SDL_DestroyRenderer(world->renderer);
  SDL_DestroyWindow(world->window);
  free(world);
  IMG_Quit();
  SDL_Quit();
  logger(INFO, __FILE_NAME__, __LINE__, "quit");
}