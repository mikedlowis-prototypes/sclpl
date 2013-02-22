/**
    @file gc.h
    @brief Public interface for the garbage collector.
    $Revision$
    $HeadURL$
*/
#ifndef GC_H
#define GC_H

#include <stdlib.h>

typedef struct {
    void*  next;
    size_t size;
} header_t;

typedef struct {
    header_t hdr;
    long     data[1];
} block_t;

void gc_set_stack_base(void* stack_base);
void* gc_allocate(size_t num_bytes);
void gc_shutdown(void);
static void  gc_collect(void);
static size_t gc_total_alloc_size(size_t size);
static block_t* gc_scan_stack(void);
static block_t* gc_scan_block(block_t* p_block, block_t* p_grey);
static block_t* gc_find_block(void* addr);
static void gc_free_blocks(block_t* p_blocks);

#endif /* GC_H */
