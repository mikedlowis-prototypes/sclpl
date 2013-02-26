#include <stdio.h>
#include "gc.h"
#include "file.h"
#include "buf.h"
#include "lex.h"

int lex_files(int argc, char** argv);
int lex_input(FILE* outfile);

int main(int argc, char** argv)
{
    int ret;
    buf_init();
    if (argc > 1)
    {
        ret = lex_files(argc,argv);
    }
    else
    {
        file_open(NULL);
        ret = lex_input(stdout);
        file_close();
    }
    return ret;
}

int lex_files(int argc, char** argv)
{
    int ret = 0;
    int i;
    for (i = 1; i < argc; i++)
    {
        if (file_open(argv[i]))
        {
            fprintf(stdout, "@file %s\n", file_name());
            ret = lex_input(stdout);
            file_close();
        }
        else
        {
            fprintf(stderr, "@error File not found: %s\n", argv[i]);
            ret = 1;
            break;
        }
    }
    return ret;
}

int lex_input(FILE* outfile)
{
    int ret = 0;
    while (!file_eof())
    {
        tok_t token = next_token();
        if (token.type != NULL)
            fprintf(outfile, "%s\t%d\t%d\t%s\n", token.type, token.line, token.column, token.str);
        free(token.str);
    }
    return ret;
}

