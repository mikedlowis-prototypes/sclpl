/**
  @file ops.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef OPS_H
#define OPS_H

#include "str.h"
#include "vec.h"

vec_t* ops_parse_file(str_t* in);
vec_t* ops_deps_file(vec_t* program);
str_t* ops_token_file(str_t* in);
str_t* ops_syntax_file(str_t* in);
str_t* ops_translate_file(str_t* in);
str_t* ops_compile_file(str_t* in);

#endif /* OPS_H */
