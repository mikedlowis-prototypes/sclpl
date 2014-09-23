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
#include <stdint.h>
#include <stddef.h>

typedef struct {
    sof_header_t*   header;
    sof_st_entry_t* symbols;
    char*           strings;
    uint8_t*        data;
    uint32_t*       code;
} sof_file_t;

sof_file_t* libsof_read_obj(char const* fname);
bool libsof_write_obj(sof_file_t* obj, char const* fname);
sof_file_t* libsof_new_obj(void);
void libsof_free_obj(sof_file_t* obj);
size_t libsof_get_symbol_table_size(sof_file_t* obj);
size_t libsof_get_string_table_size(sof_file_t* obj);
size_t libsof_get_data_segment_size(sof_file_t* obj);
size_t libsof_get_code_segment_size(sof_file_t* obj);
size_t libsof_get_num_symbols(sof_file_t* obj);
size_t libsof_add_symbol(sof_file_t* obj, const char* name, uint32_t value, uint32_t size, uint32_t info);
size_t libsof_add_st_entry(sof_file_t* obj, uint32_t name, uint32_t value, uint32_t size, uint32_t info);
sof_st_entry_t const* libsof_get_st_entry(sof_file_t* obj, size_t offset);
size_t libsof_add_string(sof_file_t* obj, char const* name);
char const* libsof_get_string(sof_file_t* obj, size_t offset);
size_t libsof_add_data(sof_file_t* obj, uint8_t const* data, size_t length);
uint8_t const* libsof_get_data(sof_file_t* obj, size_t offset);
size_t libsof_add_code(sof_file_t* obj, uint32_t const* code, size_t length);
uint32_t const* libsof_get_code(sof_file_t* obj, size_t offset);

#endif /* LIBSOF_H */
