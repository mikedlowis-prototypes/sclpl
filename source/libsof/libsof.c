/**
  @file libsof.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "libsof.h"
#include <stdint.h>
#include <stdbool.h>

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
static bool is_big_endian(void);

/******************************************************************************
 * Functions for Reading an SOF file
 *****************************************************************************/
sof_file_t* libsof_read(const char* fname)
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
        size_t sz = obj->header->sym_tbl_sz * sizeof(sof_st_entry_t);
        obj->symbols = (sof_st_entry_t*)malloc(sz);
        fread(obj->symbols, sizeof(sof_st_entry_t), obj->header->sym_tbl_sz, fhndl);
    }
}

static void libsof_read_strings(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_str_tbl_sz)
    {
        obj->str_tbl = (uint8_t*)malloc( obj->header->sym_str_tbl_sz );
        fread( obj->str_tbl, sizeof(uint8_t), obj->header->sym_str_tbl_sz, fhndl);
    }
}

static void libsof_read_data(FILE* file, sof_file_t* obj)
{
    if (obj->header->data_sz)
    {
        obj->data = (uint8_t*)malloc( obj->header->data_sz );
        fread( obj->data, sizeof(uint8_t), obj->header->data_sz, fhndl);
    }
}

static void libsof_read_code(FILE* file, sof_file_t* obj)
{
    if (obj->header->code_sz)
    {
        size_t sz = obj->header->code_sz * sizeof(uint32_t);
        obj->code = (sof_st_entry_t*)malloc(sz);
        fread(obj->code, sizeof(uint32_t), obj->header->code_sz, fhndl);
    }
}

/******************************************************************************
 * Functions for Writing an SOF file
 *****************************************************************************/
bool libsof_write(const char* fname, sof_file_t* obj)
{
    bool ret = false;
    /* Open the file for reading */
    FILE* fhndl = fopen(fname,"rb");
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
        fwrite(obj->symbols, sizeof(sof_st_entry_t), obj->header->sym_tbl_sz, fhndl);
    }
}

static void libsof_write_strings(FILE* file, sof_file_t* obj)
{
    if (obj->header->sym_str_tbl_sz)
    {
        fwrite( obj->str_tbl, sizeof(uint8_t), obj->header->sym_str_tbl_sz, fhndl);
    }
}

static void libsof_write_data(FILE* file, sof_file_t* obj)
{
    if (obj->header->data_sz)
    {
        fwrite( obj->data, sizeof(uint8_t), obj->header->data_sz, fhndl);
    }
}

static void libsof_write_code(FILE* file, sof_file_t* obj)
{
    if (obj->header->code_sz)
    {
        fwrite(obj->code, sizeof(uint32_t), obj->header->code_sz, fhndl);
    }
}

/******************************************************************************
 * Static Helper Functions
 *****************************************************************************/
static bool is_big_endian(void)
{
    union {
        uint32_t i;
        uint8_t  c[4];
    } bint = { 0x01020304 };
    return bint.c[0] == 1;
}

