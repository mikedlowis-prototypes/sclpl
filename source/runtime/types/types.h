/**
    @file types.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    long info;
} header_t;

typedef struct {
    header_t header;
    long data[1];
} object_t;

typedef const object_t* var_t;

typedef struct {
    header_t header;
    double data;
} num_t;

var_t new_num(double val);

typedef struct {
    header_t header;
    var_t first;
    var_t rest;
} cell_t;

var_t new_cell(var_t first, var_t rest);

typedef struct {
    header_t header;
    uint32_t data;
} char_t;

var_t new_char(uint32_t val);

//extern var_t True;
//extern var_t False;

#endif /* TYPES_H */
