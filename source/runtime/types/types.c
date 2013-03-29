/**
    @file types.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include "types.h"
//#include "gc.h"

var_t new_num(double val)
{
    /* Allocate the space */
    num_t* number = (num_t*)gc_allocate( sizeof(num_t) );
    /* Populate header info */
    /* Populate data */
    number->data = val;
    /* return the object */
    return (var_t)number;
}

var_t new_cell(var_t first, var_t rest)
{
    /* Allocate the space */
    cell_t* cell = (cell_t*)gc_allocate( sizeof(cell_t) );
    /* Populate header info */
    /* Populate data */
    cell->first = first;
    cell->rest = rest;
    /* return the object */
    return (var_t)cell;
}

var_t new_char(uint32_t val)
{
    /* Allocate the space */
    char_t* character = (char_t*)gc_allocate( sizeof(char_t) );
    /* Populate header info */
    /* Populate data */
    character->data = val;
    /* return the object */
    return (var_t)character;
}

