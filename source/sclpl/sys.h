/**
  @file sys.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef SYS_H
#define SYS_H

#include "str.h"

str_t* sys_bin_dir(void);
str_t* sys_inc_dir(void);

typedef enum {
    TOKFILE,
    ASTFILE,
    CSOURCE,
    OBJECT,
    PROGRAM,
    STATICLIB,
    SHAREDLIB
} file_type_t;

str_t* sys_extension(file_type_t ftype);
str_t* sys_filename(file_type_t ftype, str_t* infile);

#endif /* SYS_H */
