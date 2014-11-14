#include "sclpl.h"
#include <stdio.h>

static inline _Value make_port(FILE* file) {
    return __struct(1, ((_Value)file | 1u));
}

static inline FILE* get_file(_Value port) {
    return (FILE*)(__struct_fld(port,0) & ~1);
}

_Value __port_read_char(_Value port)
{
    return __num(fgetc(get_file(port)));
}

_Value __port_write_char(_Value port, _Value ch)
{
    fputc((int)__untag(ch), get_file(port));
    return __nil;
}

_Value __port_read_byte(_Value port)
{
    return __num(fgetc(get_file(port)));
}

_Value __port_write_byte(_Value port, _Value byte)
{
    fputc((int)__untag(byte), get_file(port));
    return __nil;
}

_Value __open_input_file(_Value fname)
{
    return make_port(fopen((char*)fname, "r"));
}

_Value __open_output_file(_Value fname)
{
    return make_port(fopen((char*)fname, "w"));
}

_Value __close_port(_Value port)
{
    fclose(get_file(port));
    return __nil;
}

_Value __is_eof(_Value port)
{
    return __bool(feof(get_file(port)));
}

