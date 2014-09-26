#include <stdio.h>
#include <stdlib.h>
#include "opts.h"
#include "grammar.h"

/* Command Line Options
 *****************************************************************************/
OptionConfig_T Options_Config[] = {
    { SHORT, (char*)"L",   (char*)"scan", 0, (char*)"Output the results of lexical analysis and quit"},
    { SHORT, (char*)"P",   (char*)"scan", 0, (char*)"Output the results of parsing quit"},
    { END,   (char*)NULL,  (char*)NULL,     0, (char*)NULL }
};

/* SCLPL Parser
 *****************************************************************************/
/* TODO:

    * Formalize grammar for parser
    * Paren for function application must be on same line as variable in REPL
    * skip line on error and terminate after full program parse
    * skip line and print on error but do not terminate the REPL

*/
int main(int argc, char **argv) {
    Result_T* results = OPTS_ParseOptions( Options_Config, argc, argv );

    parser_t* p_parser = parser_new(":> ", stdin);
    while(!parser_eof(p_parser)) {
        grammar_toplevel(p_parser);
        puts("OK.");
    }

    (void)results;
    return 0;
}
