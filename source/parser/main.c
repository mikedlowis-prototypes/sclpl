#include "gc.h"

int main(int argc, char** argv)
{
    /* init the collector */
    int foo;
    gc_set_stack_base(&foo);

    /* main program */


    /* shutdown the collector */
    gc_shutdown();
    return 0;
}

