#include <stdbool.h>
#include <stdlib.h>

#include "inc/darray.h"
#include "inc/logger.h"

struct Darray* da_new(int cap)
{
  struct Darray* darray = malloc(sizeof(struct Darray));
  if (darray == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "malloc sizeof struct Darray returned NULL");
    return NULL;
  }

  darray->cap  = cap;
  darray->len  = 0;

  darray->itmes = malloc(sizeof(void*) * cap);
  if (darray->itmes == NULL)
  {
    free(darray);
    logger(ERROR, __FILE_NAME__, __LINE__, "malloc sizeof void* x cap returned NULL");
    return NULL;
  }

  return darray;
}

bool da_add_item(struct Darray* darray, void* item)
{
  if (item == NULL)
  {
    logger(ERROR, __FILE_NAME__, __LINE__, "recived NULL items for darray add");
    return false;
  }

  if (darray->len >= darray->cap)
  {
    int new_cap      = darray->cap * 2;
    void** new_space = realloc(darray->itmes, sizeof(void*) * darray->cap);
    if (new_space == NULL)
    {
      logger(ERROR, __FILE_NAME__, __LINE__, "realloc darray items returned NULL");
      return false;
    }

    darray->cap   = new_cap;
    darray->itmes = new_space;
  }

  darray->itmes[darray->len++] = item;
  return true;
}

void da_empty(struct Darray* darray)
{
  while (darray->len > 0)
  {
    darray->itmes[darray->len] = NULL;
    darray->len--;
  }
}

void da_free(struct Darray* darray)
{
  free(darray->itmes);
  free(darray);
}
