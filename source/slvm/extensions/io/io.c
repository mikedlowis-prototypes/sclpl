/**
  @file io.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "io.h"
#include <stdio.h>
#include <pal.h>

defvar("stdin",  _stdin,  0, NULL,     0);
defvar("stdout", _stdout, 0, &_stdin,  0);
defvar("stderr", _stderr, 0, &_stdout, 0);

defconst("F_R",  f_r,  0, &_stderr, (val_t)"r");
defconst("F_W",  f_w,  0, &f_r,     (val_t)"w");
defconst("F_A",  f_a,  0, &f_w,     (val_t)"a");
defconst("F_R+", f_ru, 0, &f_a,     (val_t)"r+");
defconst("F_W+", f_wu, 0, &f_ru,    (val_t)"w+");
defconst("F_A+", f_au, 0, &f_wu,    (val_t)"a+");

defcode("fopen",  _fopen,  0, &f_au){
    *(ArgStack-1) = (val_t)fopen( (const char*)*(ArgStack-1), (const char*)*(ArgStack) );
    ArgStack--;
}

defcode("fclose", _fclose, 0, &_fopen){
    fclose((FILE*)*(ArgStack));
    ArgStack--;
}

defcode("fflush", _fflush, 0, &_fclose){
    fflush((FILE*)*(ArgStack));
    ArgStack--;
}

defcode("fgetc",  _fgetc,  0, &_fflush){
    *(ArgStack) = fgetc((FILE*)*(ArgStack));
}

defcode("fputc",  _fputc,  0, &_fgetc){
    fputc((char)*(ArgStack-1), (FILE*)*(ArgStack));
    ArgStack -= 2;
}

defcode("fputs",  _fputs,  0, &_fputc){
    fputs((char*)*(ArgStack-1), (FILE*)*(ArgStack));
    ArgStack -= 2;
}

defcode("fpeekc", _fpeekc, 0, &_fputs){
    FILE* p_file = (FILE*)*(ArgStack);
    *(ArgStack) = fgetc(p_file);
    ungetc((char)*(ArgStack), p_file);
}

dict_t* io_init(dict_t* p_prev)
{
    dict_t* p_dict = (dict_t*)pal_allocate(sizeof(dict_t));
    p_dict->name    = "io";
    p_dict->p_prev  = p_prev;
    p_dict->p_words = (word_t*)&_fpeekc;
    return p_dict;
}
