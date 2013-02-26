/**
    @file classes.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef CLASSES_H
#define CLASSES_H

#include <stdbool.h>

bool whitespace(void);
bool digit(void);
bool hex_digit(void);
bool token_end(void);
bool matches(char ch);
bool matches_any(char* str);

#endif /* CLASSES_H */
