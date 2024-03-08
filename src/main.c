#include "inc/logger.h"
#include "inc/world.h"
#include <stdlib.h>
#include <time.h>

#define TITLE "troids"

#define WINDOW_W 640
#define WINDOW_H 480

int main(void)
{
  srand(time(NULL));

  struct World* world = world_form(TITLE, WINDOW_W, WINDOW_H);
  if (world == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "world init failed");
    return 1;
  }

  world_evolve(world);
  world_free(world);

  return 0;
}
