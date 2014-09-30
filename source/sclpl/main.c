#include <stdio.h>
#include <stdlib.h>
#include "opts.h"
#include "grammar.h"

/* Command Line Options
 *****************************************************************************/
OptionConfig_T Options_Config[] = {
    { SHORT, (char*)"L",  (char*)"scan",  0, (char*)"Output the results of lexical analysis and quit"},
    { SHORT, (char*)"P",  (char*)"parse", 0, (char*)"Output the results of parsing quit"},
    { END,   (char*)NULL, (char*)NULL,    0, (char*)NULL }
};

/* SCLPL Parser
 *****************************************************************************/

void print_subtree(tree_t* p_tree, int depth);

void print_tree(vec_t* p_vec, int depth);

void print_subtree(tree_t* p_tree, int depth) {
    puts("printing subtree");
    for(int i = 0; i < (4 * depth); i++) printf("%c", ' ');
    if (p_tree->tag == ATOM) {
        printf("ATOM (%p)\n", p_tree);
    } else {
        printf("TREE (%p)\n", p_tree);
        print_tree(p_tree->ptr.vec, depth+1);
    }
    puts("printed subtree");
}

void print_tree(vec_t* p_vec, int depth) {
    puts("printing tree");
    for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
        puts("(");
        tree_t* p_tree = (tree_t*)vec_at(p_vec, idx);
        puts("-");
        print_subtree(p_tree, depth);
        puts(")");
    }
    puts("printed tree");
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
        grammar_toplevel(p_parser);
        print_tree(p_parser->p_tok_buf, 0);
        vec_clear(p_parser->p_tok_buf);
        puts("OK.");
    }
    mem_release(p_parser);

    (void)results;
    return 0;
}
