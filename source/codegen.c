#include <sclpl.h>

void codegen_init(Parser* p) {
    sym_add(&(p->syms), SF_TYPEDEF, "void",   VoidType());
    sym_add(&(p->syms), SF_TYPEDEF, "bool",   UIntType(1u));
    sym_add(&(p->syms), SF_TYPEDEF, "byte",   UIntType(8u));
    sym_add(&(p->syms), SF_TYPEDEF, "uint",   UIntType(64u));
    sym_add(&(p->syms), SF_TYPEDEF, "u8",     UIntType(8u));
    sym_add(&(p->syms), SF_TYPEDEF, "u16",    UIntType(16u));
    sym_add(&(p->syms), SF_TYPEDEF, "u32",    UIntType(32u));
    sym_add(&(p->syms), SF_TYPEDEF, "u64",    UIntType(64u));
    sym_add(&(p->syms), SF_TYPEDEF, "int",    IntType(64u));
    sym_add(&(p->syms), SF_TYPEDEF, "i8",     IntType(8u));
    sym_add(&(p->syms), SF_TYPEDEF, "i16",    IntType(16u));
    sym_add(&(p->syms), SF_TYPEDEF, "i32",    IntType(32u));
    sym_add(&(p->syms), SF_TYPEDEF, "i64",    IntType(64u));
    sym_add(&(p->syms), SF_TYPEDEF, "string",
        ArrayOf(sym_get(&(p->syms), "byte")->type, -1));
}
