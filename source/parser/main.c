#include <stdio.h>
#include "tok.h"

int parse_files(int num_files, char** fnames);
int parse_input(char* outfile);

int main(int argc, char** argv)
{
    int ret;
    if (argc > 1)
    {
        ret = parse_files(argc,argv);
    }
    else
    {
        ret = parse_input(NULL);
    }
    return ret;
}

int parse_files(int num_files, char** fnames)
{
    int ret = 0;
    int i;
    for (i = 1; i < num_files; i++)
    {
        parse_input(fnames[i]);
    }
}

int parse_input(char* outfile)
{
    int ret = 0;
    if (tok_source(outfile))
    {
        while (!tok_eof())
        {
            tok_t* p_tok = tok_read();
            if (NULL != p_tok)
            {
                printf( "%s:%d:%d:\t%d\t%s",
                        p_tok->p_file_name,
                        p_tok->line,
                        p_tok->column,
                        p_tok->type,
                        p_tok->p_text );
            }
        }
        tok_finish();
    }
    return ret;
}


