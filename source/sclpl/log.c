/**
  @file log.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "log.h"
#include <stdio.h>

void log_error(const char msg[], ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fputs("\n",stderr);
}

