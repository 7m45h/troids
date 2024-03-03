#include <stdio.h>

#include "inc/logger.h"

static const char* icon[2] = { "info", "error" };

void logger(enum LogType type, const char* file, const int line, const char* msg)
{
  printf("[%s] %s:%d %s\n", icon[type], file, line, msg);
}
