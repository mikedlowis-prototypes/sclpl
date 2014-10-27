
/**
  @file sclpl.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
*/
#ifndef SCLPL_H
#define SCLPL_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#define IF(a) (a) ?
#define ELSE  :

#define BITCOUNT         (sizeof(void*) * 8u)
#define REFCOUNT_MASK    ((1u << (BITCOUNT/2u)) - 1u)
#define RECCOUNT_MASK    ((intptr_t)-1u)
#define GET_RECCOUNT(a)  (((a) & RECCOUNT_MASK) >> (BITCOUNT/2))
#define MAKE_RECCOUNT(a) ((a) << (BITCOUNT/2))
#define GET_REFCOUNT(a)  ((a) & REFCOUNT_MASK)

//#define __is_ptr(v) ((bool)((v & 1u) == 0))
//#define __is_num(v) ((bool)((v & 1u) == 1))

#define __nil     ((_Value)NULL)
#define __num(v)  ((_Value)(((intptr_t)(v) << 1u) | 1u))
#define __int(v)  __num(v)
#define __char(v) __num(v)
#define __bool(v) __num(v)

typedef struct {
    uintptr_t refcount;
} _Object;

typedef intptr_t _Value;

static inline void* allocate(size_t nflds, size_t size)
{
    _Object* p_obj = (_Object*)malloc(sizeof(_Object) + size);
    p_obj->refcount = MAKE_RECCOUNT(nflds) | 1;
    return (void*)(p_obj+1);
}

static inline _Value retain(_Value val)
{
    assert( val && !(val & 1u) );
    (((_Object*)val)-1)->refcount++;
    return val;
}

static inline _Value release(_Value val)
{
    assert( val && !(val & 1u) );
    (((_Object*)val)-1)->refcount--;
    return __nil;
}

static inline _Value reccount(_Value val)
{
    assert( val && !(val & 1u) );
    return __num( (((_Object*)val)-1)->refcount >> (BITCOUNT/2) );
}

static inline _Value refcount(_Value val)
{
    assert( val && !(val & 1u) );
    return __num( (((_Object*)val)-1)->refcount );
}

static inline _Value __float(double v) {
    double* dbl = (double*)allocate(0, sizeof(double));
    *dbl = v;
    return (_Value)dbl;
}

static inline _Value __string(char v[]) {
    size_t sz = strlen(v)+1;
    char* str = (char*)allocate(0, sz);
    (void)memcpy(str, v, sz);
    return (_Value)str;
}

static inline _Value __struct(size_t nflds, ...) {
    void** obj = (void**)allocate(nflds, sizeof(void*) * nflds);
    va_list args;
    va_start(args, nflds);
    for(size_t i = 0; i < nflds; i++)
        obj[i] = va_arg(args, void*);
    va_end(args);
    return (_Value)obj;
}

#define __struct_fld(val, idx) (((_Value*)val)[idx])

#define __func(fn)              __struct(1, fn)

#define __closure(fn,nfree,...) __struct(nfree, fn, __VA_ARGS__)

#define __call0(fn)             ((__fnptr_0)(__struct_fld(fn,0) & ~1u))(fn)

#define __calln(fn,nargs,...)   ((__fnptr_##nargs)(__struct_fld(fn,0) & ~1u))(fn, __VA_ARGS__)

typedef _Value (*__fnptr_0)(_Value env);

typedef _Value (*__fnptr_1)(_Value env, _Value a0);

typedef _Value (*__fnptr_2)(_Value env, _Value a0, _Value a1);

typedef _Value (*__fnptr_3)(_Value env, _Value a0, _Value a1, _Value a2);

typedef _Value (*__fnptr_4)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3);

typedef _Value (*__fnptr_5)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4);

typedef _Value (*__fnptr_6)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5);

typedef _Value (*__fnptr_7)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6);

typedef _Value (*__fnptr_8)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7);

typedef _Value (*__fnptr_9)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8);

typedef _Value (*__fnptr_10)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8);

typedef _Value (*__fnptr_11)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9);

typedef _Value (*__fnptr_12)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9, _Value a10);

typedef _Value (*__fnptr_13)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9, _Value a10, _Value a11);

typedef _Value (*__fnptr_14)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9, _Value a10, _Value a11, _Value a12);

typedef _Value (*__fnptr_15)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9, _Value a10, _Value a11, _Value a12, _Value a14);

typedef _Value (*__fnptr_16)(_Value env, _Value a0, _Value a1, _Value a2, _Value a3,
    _Value a4, _Value a5, _Value a6, _Value a7, _Value a8,
    _Value a9, _Value a10, _Value a11, _Value a12, _Value a14,
    _Value a15);

typedef _Value (*__fnptr_n)(_Value env, ...);

/*****************************************************************************/

/* Boolean Operations */
#define __not(val) __num(!((bool)__untag(val)))

/* Character procedures */
#define __char_lt(lval, rval)  __ilt((lval), (rval))
#define __char_gt(lval, rval)  __igt((lval), (rval))
#define __char_eq(lval, rval)  __ieq((lval), (rval))
#define __char_lte(lval, rval) __ilte((lval), (rval))
#define __char_gte(lval, rval) __igte((lval), (rval))
#define __char_upcase(val)     assert(false)
#define __char_downcase(val)   assert(false)
#define __char_foldcase(val)   assert(false)

/* Integer Operations */
#define __untag(a) ((a) >> 1u)
#define __iadd(lval, rval) __num(__untag(lval) + __untag(rval))
#define __isub(lval, rval) __num(__untag(lval) - __untag(rval))
#define __imul(lval, rval) __num(__untag(lval) * __untag(rval))
#define __idiv(lval, rval) __num(__untag(lval) / __untag(rval))
#define __imod(lval, rval) __num(__untag(lval) % __untag(rval))
#define __ilt(lval, rval)  __bool(__untag(lval) < __untag(rval))
#define __igt(lval, rval)  __bool(__untag(lval) > __untag(rval))
#define __ieq(lval, rval)  __bool(__untag(lval) == __untag(rval))
#define __ilte(lval, rval) __bool(__untag(lval) <= __untag(rval))
#define __igte(lval, rval) __bool(__untag(lval) >= __untag(rval))

/* Float Operations */
#define __fadd(lval, rval) __float(*lval + *rval)
#define __fsub(lval, rval) __float(*lval - *rval)
#define __fmul(lval, rval) __float(*lval * *rval)
#define __fdiv(lval, rval) __float(*lval / *rval)
#define __fmod(lval, rval) __float(*lval % *rval)
#define __flt(lval, rval)  __bool(*lval < *rval)
#define __fgt(lval, rval)  __bool(*lval > *rval)
#define __feq(lval, rval)  __bool(*lval == *rval)
#define __flte(lval, rval) __bool(*lval <= *rval)
#define __fgte(lval, rval) __bool(*lval >= *rval)

/* String Operations */
#define __string_length(val)         strlen((char*)val)
#define __string_ref(str, idx)       __num(((char*)val)[idx])
#define __string_set(str, idx, ch)   (((char*)val)[idx] = ch, __nil)
#define __string_eq(lval, rval)      __num(0 == strcmp((char*)lval, (char*)rval))
#define __string_lt(lval, rval)      __num(-1 == strcmp((char*)lval, (char*)rval))
#define __string_gt(lval, rval)      __num(1 == strcmp((char*)lval, (char*)rval))
#define __string_lte(lval, rval)     __num(__string_lt(lval, rval) || __string_eq(lval, rval))
#define __string_gte(lval, rval)     __num(__string_gt(lval, rval) || __string_eq(lval, rval))
#define __string_ci_eq(lval, rval)   assert(false)
#define __string_ci_lt(lval, rval)   assert(false)
#define __string_ci_gt(lval, rval)   assert(false)
#define __string_ci_lte(lval, rval)  assert(false)
#define __string_ci_gte(lval, rval)  assert(false)
#define __string_upcase(val, rval)   assert(false)
#define __string_downcase(val, rval) assert(false)
#define __string_foldcase(val, rval) assert(false)
#define __substring(val, start, end) assert(false)
#define __string_concat(lval, rval)  assert(false)

#endif /* SCLPL_H */
