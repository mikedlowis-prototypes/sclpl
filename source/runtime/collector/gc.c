#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <setjmp.h>
#include "gc.h"

/*****************************************************************************/
void* p_Stack_Base     = NULL;
block_t* p_Blocks      = NULL;
size_t Bytes_Allocated = 0;

/*****************************************************************************/
void gc_set_stack_base(void* stack_base)
{
    p_Stack_Base = stack_base;
}

void* gc_allocate(size_t num_bytes)
{
    /* If we need to, then trigger a collection */
    if (Bytes_Allocated >= (1024 * 1024)/* 1 MB */)
        gc_collect();

    /* Allocate the block */
    size_t   alloc_sz = gc_total_alloc_size(num_bytes);
    block_t* p_block  = (block_t*) malloc( alloc_sz );

    /* Set the metadata */
    Bytes_Allocated  += alloc_sz;
    p_block->hdr.size = alloc_sz - sizeof(header_t);
    p_block->hdr.next = p_Blocks;
    p_Blocks          = p_block;

    /* Return the data portion */
    return &(p_block->data[0]);
}

void gc_shutdown(void)
{
    gc_free_blocks(p_Blocks);
}

void gc_collect(void)
{
    if (NULL != p_Stack_Base)
    {
        block_t* p_grey  = gc_scan_stack();
        block_t* p_black = NULL;
        Bytes_Allocated  = 0;

        while(p_grey != NULL)
        {
            /* Remove the block from the grey list */
            block_t* p_block = p_grey;
            p_grey = p_grey->hdr.next;

            /* add it to the black list */
            p_block->hdr.next = p_black;
            p_black = p_block;

            /* scan the block for more objects */
            p_grey = gc_scan_block(p_block,p_grey);
        }

        /* Reclaim all unused blocks */
        gc_free_blocks(p_Blocks);
        p_Blocks = p_black;
    }
}

size_t gc_total_alloc_size(size_t size)
{
    /* Get the total number of bytes to allocate (including block metadata) */
    size_t req_size = sizeof(header_t) + size;
    /* Align to a machine word boundary. 4 bytes on 32-bit, 8 bytes on 64-bit */
    size_t align = (sizeof(long) - (req_size % sizeof(long)));
    align = (align == sizeof(long)) ? 0 : align;
    req_size = req_size + align;
    return req_size;
}

block_t* gc_scan_stack(void)
{
    jmp_buf  registers;
    void**   stack_top;
    void**   stack_btm;
    block_t* p_roots = NULL;

    /* Figure out which direction the stack grows and set the pointers accordingly */
    stack_btm = ((p_Stack_Base < (void*)&stack_top) ? p_Stack_Base : &stack_top);
    stack_top = ((p_Stack_Base < (void*)&stack_top) ? &stack_top : p_Stack_Base);

    /* Copy register info to the stack */
    (void)setjmp( registers );

    /* scan the stack for pointers */
    while (stack_btm < stack_top)
    {
        /* Search the block list for a the block referenced */
        block_t* p_block = gc_find_block( stack_btm[0] );

        /* If we found a valid pointer */
        if (p_block != NULL)
        {
            /* The block is a root */
            p_block->hdr.next = p_roots;
            p_roots = p_block;
        }

        /* continue */
        stack_btm++;
    }

    return p_roots;
}

block_t* gc_scan_block(block_t* p_block, block_t* p_grey)
{
    size_t index = p_block->hdr.size / sizeof(long);
    for (index; index > 0; index--)
    {
        void* ptr = (void*)(p_block->data[index]);
        block_t* p_block = gc_find_block( ptr );
        if (p_block != NULL)
        {
            p_block->hdr.next = p_grey;
            p_grey = p_block;
            break;
        }
    }
    return p_grey;
}

block_t* gc_find_block(void* addr)
{
    block_t* p_block = NULL;
    block_t* p_prev  = NULL;
    block_t* p_curr  = p_Blocks;
    while (p_curr != NULL)
    {
        if (p_curr->data == addr)
        {
            p_block = p_curr;
            /* If the block is at the head of the list */
            if (p_prev == NULL)
            {
                p_prev   = p_block->hdr.next;
                p_Blocks = p_prev;
            }
            /* Else its somewhere in the list */
            else
            {
                p_prev->hdr.next = p_block->hdr.next;
            }
            /* Clear the blocks next pointer */
            p_block->hdr.next = NULL;
            break;
        }
        p_prev = p_curr;
        p_curr = p_curr->hdr.next;
    }
    return p_block;
}

void gc_free_blocks(block_t* p_blocks)
{
    while (p_blocks != NULL)
    {
        block_t* p_block = p_blocks;
        p_blocks = p_blocks->hdr.next;
        free(p_block);
    }
}

/*****************************************************************************/
#if 0
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv)
{
    /* Give the garbage collector a pointer to the base of our stack */
    int stack_base = 0;
    gc_set_stack_base(&stack_base);

    /* setup test variables */
    #define STACK_REF_ARRY_SZ 20
    int stack_ref_idx = 0;
    void* stack_refs[ STACK_REF_ARRY_SZ ] = { 0u };

    /* Generate some random block allocation requests */
    srand(time(NULL));
    while(true)
    {
        size_t size = rand();
        void* new_obj = gc_allocate( size );
        if (0 == (rand() % 2))
        {
            stack_refs[ stack_ref_idx % STACK_REF_ARRY_SZ ] = new_obj;
            stack_ref_idx++;
        }
        (void)stack_refs;
    }

    return 0;
}
#endif

