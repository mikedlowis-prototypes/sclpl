#include "core.h"
#include "pal.h"
#include <stdlib.h>

#if 0
defcode("if", _if, 1, &bytemove){
    /* Compile branch instruction */
    ArgStack++;
    *(ArgStack) = (val_t)&zbranch;
    EXEC(comma);

    /* Save off the current offset */
    EXEC(here);

    /* Compile a dummy offset */
    ArgStack++;
    *(ArgStack) = (val_t)0;
    EXEC(comma);
}

defcode("then", _then, 1, &_if){
    /* calculate the address where the offset should be stored */
    EXEC(swap);
    EXEC(dup);
    EXEC(wcode);
    EXEC(nrot);

    /* Calculate the branch offset */
    EXEC(dup);
    EXEC(here);
    EXEC(swap);
    EXEC(sub);

    /* Store the offset */
    EXEC(swap);
    EXEC(wordsz);
    EXEC(mul);
    EXEC(nrot);
    EXEC(add);
    EXEC(store);
}

defcode("else", _else, 1, &_then){
    /* Compile the branch instruction */
    ArgStack++;
    *(ArgStack) = (val_t)&branch;
    EXEC(comma);

    /* Save off the current offset */
    EXEC(here);
    EXEC(rot);

    /* Compile a dummy offset */
    ArgStack++;
    *(ArgStack) = 0;
    EXEC(comma);

    /* Set the branch offset for the first branch */
    EXEC(_then);
    EXEC(swap);
}
#endif

dict_t* core_init(dict_t* p_prev)
{
    dict_t* p_dict = (dict_t*)pal_allocate(sizeof(dict_t));
    p_dict->name    = "core";
    p_dict->p_prev  = p_prev;
    p_dict->p_words = (word_t*)NULL;
    //p_dict->p_words = (word_t*)&_fpeekc;
    return p_dict;
}
