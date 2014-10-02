/**
  @file scanner.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char* p_line;
    size_t index;
    FILE* p_input;
    char* p_prompt;
} scanner_t;

scanner_t* scanner_new(char* p_prompt, FILE* p_file);

char* scanner_read(scanner_t* p_scanner);

bool scanner_eof(scanner_t* p_scanner);

bool scanner_eol(scanner_t* p_scanner);

void scanner_getline(scanner_t* p_scanner);

#endif /* SCANNER_H */
