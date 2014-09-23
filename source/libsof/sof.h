/**
  @file sof.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef SOF_H
#define SOF_H

#include <stdint.h>

/*
    SCLPL Object File Layout

    -----------------------
    | SOF Header          |
    -----------------------
    | Symbol Table        |
    -----------------------
    | Symbol String Table |
    -----------------------
    | Data Segment        |
    -----------------------
    | Code Segment        |
    -----------------------

*/

/* Macro for generating a 32-bit date code based on year, month, and day */
#define DATE_CODE(year,month,day) (year << 16) | (month << 8) | (day)

/* The version of the SOF format supported by this library represented as a
 * 32-bit date code */
#define SOF_VERSION DATE_CODE(2013,9,10)

/* Definition of the SOF file header. The header appears at the beginning of any
 * SOF file and contains information about the SOF version of the file and the
 * sizes of each section in the file. */
typedef struct {
    /* 32-bit date code representing the version of SOF format used by the file */
    uint32_t version;
    /* This size of the symbol table in bytes. A value of 0 indicates that the
     * symbol table segment has been omitted from the file */
    uint32_t sym_tbl_sz;
    /* The size of the symbol string table segment in bytes. Each entry in the
     * symbol string table consists of an array of bytes terminated by a NULL
     * byte (0x00). */
    uint32_t sym_str_tbl_sz;
    /* The size in bytes of the constant data segment. This segment contains
     * constant data that is referenced by the code segment. */
    uint32_t data_sz;
    /* The size of the code segment in bytes. Each instruction is represented by
     * a 32-bit value and represents a single action to be performed by the
     * bytecode interpreter. */
    uint32_t code_sz;
} sof_header_t;

/* Definition of the SOF symbol table entry */
typedef struct {
    /* Offset into the string section where the string for the symbol is
     * located */
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint32_t info;
} sof_st_entry_t;

#endif /* SOF_H */
