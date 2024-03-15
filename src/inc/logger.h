#pragma once

enum LogType
{
  INFO,
  ERROR,
  WARNING
};

void logger(enum LogType type, const char* file, const int line, const char* msg);
