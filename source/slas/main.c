#include <stdio.h>

static int assemble_file(char* infile, char* outfile);
static int generate_bytecode(FILE* ifh, FILE* ofh);

static char usage[] =
    "\nUsage: slas <INFILE> <OUTFILE>\n\n"
    "Assemble <INFILE> to SCLPL bytecode and write the result to <OUTFILE>.\n" ;

static char buffer[1024] = { 0 };

int main(int argc, char** argv)
{
    int ret = 0;
    if (argc == 3)
    {
        ret = assemble_file(argv[1],argv[2]);
    }
    else
    {
        puts(usage);
    }
    return ret;
}

static int assemble_file(char* infile, char* outfile)
{
    int ret = 1;
    FILE* ifh = fopen(infile,  "r");
    FILE* ofh = fopen(outfile, "w");
    if (ifh && ofh)
    {
        ret = generate_bytecode(ifh,ofh);
    }
    fclose(ifh);
    fclose(ofh);
    fclose(NULL);
    return ret;
}

static int generate_bytecode(FILE* ifh, FILE* ofh)
{
    int ret = 0;
    //asm_ctx_t context;
    //asm_init(&context);
    while(!feof(ifh))
    {
        if (fgets(buffer,1023,ifh))
        {
            puts(buffer);
        }
        else
        {
            ret = 1;
            break;
        }
    }
    return ret;
}


