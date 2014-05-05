/**
  @file pal.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
*/
#ifndef PAL_H
#define PAL_H

#include "slvm.h"
#include <stdbool.h>
#include <string.h>

extern val_t* ArgStack;
extern val_t* CodePtr;

dict_t* pal_init(dict_t* p_prev_dict);
void    pal_prompt(void);
void*   pal_allocate(size_t size);
void*   pal_reallocate(void* p_mem, size_t size);
void    pal_free(void* p_mem);
char    pal_read_char(void);
char    pal_peek_char(void);
bool    pal_is_eof(void);

#endif /* PAL_H */
