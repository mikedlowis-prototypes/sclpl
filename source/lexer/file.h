/**
    @file file.h
    @brief Describes a simple module for tracking input from a file including
           line and column info.
    $Revision$
    $HeadURL$
*/
#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdbool.h>

bool file_open(char* fname);
void file_close(void);
bool file_eof(void);
char file_get(void);
char file_peek(void);
int file_line(void);
int file_column(void);
char* file_name(void);

#endif /* FILE_H */
