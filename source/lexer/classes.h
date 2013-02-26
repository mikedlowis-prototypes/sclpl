/**
    @file classes.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef CLASSES_H
#define CLASSES_H

#include <stdbool.h>

typedef bool (*predicate_t)(void);

bool whitespace(void);
bool digit(void);
bool hex_digit(void);
bool token_end(void);
bool matches(char ch);
bool matches_any(char* str);
bool one_or_more(predicate_t pfn);

#endif /* CLASSES_H */
