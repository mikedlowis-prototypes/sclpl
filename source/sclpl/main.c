#include <stdio.h>
#include <stdlib.h>
#include "opts.h"
#include "grammar.h"
#include "lexer.h"

/* Command Line Options
 *****************************************************************************/
OptionConfig_T Options_Config[] = {
    { SHORT, (char*)"L",  (char*)"scan",  0, (char*)"Output the results of lexical analysis and quit"},
    { SHORT, (char*)"P",  (char*)"parse", 0, (char*)"Output the results of parsing quit"},
    { END,   (char*)NULL, (char*)NULL,    0, (char*)NULL }
};

/* SCLPL Parser
 *****************************************************************************/

void print_indent(int depth) {
    for(int i = 0; i < (2 * depth); i++)
        printf("%c", ' ');
}

void print_tree(tree_t* p_tree, int depth) {
    print_indent(depth);
    if (p_tree->tag == ATOM) {
        lex_tok_t* p_tok = p_tree->ptr.tok;
        printf("<token(%s)>\n", lexer_tok_type_str(p_tok));
    } else {
        puts("(tree:");
        vec_t* p_vec = p_tree->ptr.vec;
        for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
            print_tree((tree_t*)vec_at(p_vec, idx), depth+1);
        }
        print_indent(depth);
        puts(")");
    }
}

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
        tree_t* p_tree = grammar_toplevel(p_parser);
        print_tree(p_tree, 0);
        mem_release(p_tree);
        puts("OK.");
    }
    mem_release(p_parser);

    (void)results;
    return 0;
}
