#include <stdio.h>

#include "inc/logger.h"

static const char* icon[] = { "info", "error", "warning" };

void logger(enum LogType type, const char* file, const int line, const char* msg)
{
  printf("[%s] %s:%d %s\n", icon[type], file, line, msg);
}
