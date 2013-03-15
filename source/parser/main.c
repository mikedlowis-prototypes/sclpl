#include <stdio.h>

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
    //tok_open(outfile);

    //tok_close();
}


