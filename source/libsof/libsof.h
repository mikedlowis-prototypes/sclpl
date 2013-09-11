/**
  @file libsof.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef LIBSOF_H
#define LIBSOF_H

#include "sof.h"
#include <stdbool.h>

typedef struct {
    sof_header_t*   header;
    sof_st_entry_t* symbols;
    uint8_t*        str_tbl;
    uint8_t*        data;
    uint32_t*       code;
} sof_file_t;

sof_file_t* libsof_read(const char* fname);
bool libsof_write(const char* fname, sof_file_t* obj);

#endif /* LIBSOF_H */
