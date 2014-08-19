/**
  @file libsof.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "libsof.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void libsof_read_header(FILE* file, sof_file_t* obj);
static void libsof_read_symbols(FILE* file, sof_file_t* obj);
static void libsof_read_strings(FILE* file, sof_file_t* obj);
static void libsof_read_data(FILE* file, sof_file_t* obj);
static void libsof_read_code(FILE* file, sof_file_t* obj);
static void libsof_write_header(FILE* file, sof_file_t* obj);
static void libsof_write_symbols(FILE* file, sof_file_t* obj);
static void libsof_write_strings(FILE* file, sof_file_t* obj);
static void libsof_write_data(FILE* file, sof_file_t* obj);
static void libsof_write_code(FILE* file, sof_file_t* obj);
static void* libsof_get_segment_addr(void* segment, size_t seg_size, size_t el_size, size_t offset);
static size_t libsof_add_to_segment(void** segment, size_t* seg_size, void const* data, size_t length);

/******************************************************************************
 * Functions for Reading an SOF file
 *****************************************************************************/
sof_file_t* libsof_read_obj(char const* fname)
{
    sof_file_t* obj = NULL;
    /* Open the file for reading */
    FILE* fhndl = fopen(fname,"rb");
    /* read the files contents if valid */
    if (fhndl)
    {
        /* Allocate space for the  header and object structure */
        obj = (sof_file_t*)calloc(1,sizeof(sof_file_t));
        /* Read the contents of the file into memory */
        libsof_read_header(fhndl, obj);
        libsof_read_symbols(fhndl, obj);
        libsof_read_strings(fhndl, obj);
        libsof_read_data(fhndl, obj);
        libsof_read_code(fhndl, obj);
    }
    /* close the file and return the read object */
    fclose(fhndl);
    return obj;
}

static void libsof_read_header(FILE* file, sof_file_t* obj)
{
    /* Read the object header out of the file */
    obj->header = (sof_header_t*)malloc(sizeof(sof_header_t));
    fread(obj->header, sizeof(sof_header_t), 1, file);
}

static void libsof_read_symbols(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_tbl_sz)
    {
        obj->symbols = (sof_st_entry_t*)malloc( obj->header->sym_tbl_sz );
        fread(obj->symbols, sizeof(uint8_t), obj->header->sym_tbl_sz, file);
    }
}

static void libsof_read_strings(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_str_tbl_sz)
    {
        obj->strings = (char*)malloc( obj->header->sym_str_tbl_sz );
        fread( obj->strings, sizeof(uint8_t), obj->header->sym_str_tbl_sz, file);
    }
}

static void libsof_read_data(FILE* file, sof_file_t* obj)
{
    if (obj->header->data_sz)
    {
        obj->data = (uint8_t*)malloc( obj->header->data_sz );
        fread( obj->data, sizeof(uint8_t), obj->header->data_sz, file);
    }
}

static void libsof_read_code(FILE* file, sof_file_t* obj)
{
    if (obj->header->code_sz)
    {
        obj->code = (uint32_t*)malloc( obj->header->code_sz );
        fread(obj->code, sizeof(uint8_t), obj->header->code_sz, file);
    }
}

/******************************************************************************
 * Functions for Writing an SOF file
 *****************************************************************************/
bool libsof_write_obj(sof_file_t* obj, char const* fname)
{
    bool ret = false;
    /* Open the file for reading */
    FILE* fhndl = fopen(fname,"wb");
    /* if the file was successfully opened */
    if (fhndl)
    {
        /* Write the contents of the file in sequence */
        libsof_write_header(fhndl,obj);
        libsof_write_symbols(fhndl,obj);
        libsof_write_strings(fhndl,obj);
        libsof_write_data(fhndl,obj);
        libsof_write_code(fhndl,obj);
    }
    /* close the file and return the read object */
    fclose(fhndl);
    return ret;
}

static void libsof_write_header(FILE* file, sof_file_t* obj)
{
    fwrite(obj->header, sizeof(sof_header_t), 1, file);
}

static void libsof_write_symbols(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_tbl_sz)
    {
        fwrite(obj->symbols, sizeof(sof_st_entry_t), obj->header->sym_tbl_sz / sizeof(sof_st_entry_t), file);
    }
}

static void libsof_write_strings(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_str_tbl_sz)
    {
        fwrite( obj->strings, sizeof(uint8_t), obj->header->sym_str_tbl_sz, file);
    }
}

static void libsof_write_data(FILE* file, sof_file_t* obj)
{
    if (obj->header->data_sz)
    {
        fwrite( obj->data, sizeof(uint8_t), obj->header->data_sz, file);
    }
}

static void libsof_write_code(FILE* file, sof_file_t* obj)
{
    if (obj->header->code_sz)
    {
        fwrite(obj->code, sizeof(uint32_t), obj->header->code_sz / sizeof(uint32_t), file);
    }
}

/******************************************************************************
 * Functions for Creating and Modifying SOF files
 *****************************************************************************/
sof_file_t* libsof_new_obj(void)
{
    sof_file_t* obj      = (sof_file_t*)calloc(1,sizeof(sof_file_t));
    obj->header          = (sof_header_t*)calloc(1,sizeof(sof_header_t));
    obj->header->version = SOF_VERSION;
    return obj;
}

void libsof_free_obj(sof_file_t* obj)
{
    free(obj->header);
    free(obj->symbols);
    free(obj->strings);
    free(obj->data);
    free(obj->code);
    free(obj);
}

size_t libsof_get_symbol_table_size(sof_file_t* obj)
{
    return obj->header->sym_tbl_sz;
}

size_t libsof_get_string_table_size(sof_file_t* obj)
{
    return obj->header->sym_str_tbl_sz;
}

size_t libsof_get_data_segment_size(sof_file_t* obj)
{
    return obj->header->data_sz;
}

size_t libsof_get_code_segment_size(sof_file_t* obj)
{
    return obj->header->code_sz;
}

size_t libsof_get_num_symbols(sof_file_t* obj)
{
    return obj->header->sym_tbl_sz / sizeof(sof_st_entry_t);
}

size_t libsof_add_symbol(sof_file_t* obj, const char* name, uint32_t value, uint32_t size, uint32_t info)
{
    size_t str_idx = libsof_add_string(obj, name);
    return libsof_add_st_entry(obj, str_idx, value, size, info);
}

size_t libsof_add_st_entry(sof_file_t* obj, uint32_t name, uint32_t value, uint32_t size, uint32_t info)
{
    sof_st_entry_t new_sym = { name, value, size, info };
    return libsof_add_to_segment( (void**)&(obj->symbols), (size_t*)&(obj->header->sym_tbl_sz), &new_sym, sizeof(sof_st_entry_t) );
}

sof_st_entry_t const* libsof_get_st_entry(sof_file_t* obj, size_t offset)
{
    return libsof_get_segment_addr( obj->symbols, obj->header->sym_tbl_sz, sizeof(sof_st_entry_t), offset);
}

size_t libsof_add_string(sof_file_t* obj, char const* name)
{
    return libsof_add_to_segment( (void**)&(obj->strings), (size_t*)&(obj->header->sym_str_tbl_sz), name, strlen(name) + 1 );
}

char const* libsof_get_string(sof_file_t* obj, size_t offset)
{
    return libsof_get_segment_addr( obj->strings, obj->header->sym_str_tbl_sz, sizeof(char), offset);
}

size_t libsof_add_data(sof_file_t* obj, uint8_t const* data, size_t length)
{
    return libsof_add_to_segment( (void**)&(obj->data), (size_t*)&(obj->header->data_sz), data, length );
}

uint8_t const* libsof_get_data(sof_file_t* obj, size_t offset)
{
    return libsof_get_segment_addr( obj->data, obj->header->data_sz, sizeof(uint8_t), offset);
}

size_t libsof_add_code(sof_file_t* obj, uint32_t const* code, size_t length)
{
    return libsof_add_to_segment( (void**)&(obj->code), (size_t*)&(obj->header->code_sz), code, length * sizeof(uint32_t) );
}

uint32_t const* libsof_get_code(sof_file_t* obj, size_t offset)
{
    return libsof_get_segment_addr( obj->code, obj->header->code_sz, sizeof(uint32_t), offset);
}

/******************************************************************************
 * Static Helper Functions
 *****************************************************************************/
static void* libsof_get_segment_addr(void* segment, size_t seg_size, size_t el_size, size_t offset)
{
    void* addr = NULL;
    size_t addr_offset = offset * el_size;
    if (addr_offset < seg_size)
    {
        addr = segment + addr_offset;
    }
    return addr;
}

static size_t libsof_add_to_segment(void** segment, size_t* seg_size, void const* data, size_t length)
{
    size_t offset   = *(seg_size);
    *(seg_size) = offset + length;
    *(segment) = realloc(*(segment), *(seg_size));
    memcpy( *(segment) + offset, data, length );
    return offset;
}

