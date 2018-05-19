/**
  @file vec.c
*/
#include <sclpl.h>

#ifndef DEFAULT_VEC_CAPACITY
#define DEFAULT_VEC_CAPACITY (size_t)8
#endif

void vec_init(vec_t* vec)
{
    vec->count    = 0;
    vec->capacity = DEFAULT_VEC_CAPACITY;
    vec->buffer   = malloc(sizeof(void*) * vec->capacity);
}

void vec_deinit(vec_t* vec)
{
    vec_clear(vec);
    free(vec->buffer);
}

size_t vec_size(vec_t* vec)
{
    return vec->count;
}

void* vec_at(vec_t* vec, size_t index)
{
    assert(index < vec->count);
    return vec->buffer[index];
}

static void vec_reserve(vec_t* vec, size_t size)
{
    assert(vec != NULL);
    vec->buffer = realloc( vec->buffer, sizeof(void*) * size );
    assert(vec->buffer != NULL);
    vec->capacity = size;
}

static size_t vec_next_capacity(size_t req_size)
{
    size_t next_power = req_size;
    size_t num_bits = sizeof(size_t) * 8;
    size_t bit_n;
    /* Find the next highest power of 2 */
    next_power--;
    for (bit_n = 1; bit_n < num_bits; bit_n = bit_n << 1)
    {
        next_power = next_power | (next_power >> bit_n);
    }
    next_power++;
    return next_power;
}

static void vec_resize(vec_t* vec, size_t count, void* fillval)
{
    if (count > vec->count) {
        vec_reserve(vec, vec_next_capacity(count+1));
        for (; vec->count < count; vec->count++)
            vec->buffer[vec->count] = fillval;
    } else if (count < vec->count) {
        vec->count = count;
    }
}

void vec_push_back(vec_t* vec, void* data)
{
    vec_resize(vec, vec->count+1, data);
}

void vec_set(vec_t* vec, size_t index, void* data)
{
    assert(index < vec->count);
    vec->buffer[index] = data;
}

void vec_clear(vec_t* vec)
{
    vec->count = 0;
}

