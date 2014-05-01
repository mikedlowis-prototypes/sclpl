/**
  @file parser.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef PARSER_H
#define PARSER_H

#include "slvm.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    ERROR = 0,
    WORD,
    STRING,
    CHAR,
    INTEGER,
    FLOAT,
} TokenType_T;

char* fetch_token(FILE* input);

TokenType_T parse(char* str, val_t* p_val);

bool line_read(void);

#endif /* PARSER_H */
