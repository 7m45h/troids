#pragma once

#include <stdbool.h>

#define DEFAULT_DARRAY_CAP 16

struct Darray
{
  int cap;
  int len;
  void** itmes;
};

struct Darray* da_new(int cap);
bool da_add_item(struct Darray* darray, void* item);
void da_empty(struct Darray* darray);
void da_free(struct Darray* darray);
