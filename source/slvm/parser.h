/**
  @file parser.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef PARSER_H
#define PARSER_H

#include "slvm.h"

typedef enum {
    WORD = 0,
    STRING,
    CHAR,
    INTEGER,
    FLOAT,
    ERROR
} TokenType_T;

char* fetch(FILE* input);

ValueType_T parse(char* str, val_t* p_val);

#endif /* PARSER_H */
