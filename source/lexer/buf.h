/**
    @file buf.h
    @brief A simple string building buffer akin to string streams in c++.
    $Revision$
    $HeadURL$
*/
#ifndef BUF_H
#define BUF_H

#include <string.h>

void buf_init(void);
void buf_put(char ch);
char* buf_accept(void);
void buf_grow(void);

#endif /* BUF_H */
