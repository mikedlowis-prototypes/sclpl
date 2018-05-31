#include <sclpl.h>

static Type* mktype(Kind kind) {
    Type* type = emalloc(sizeof(Type));
    memset(type, 0, sizeof(Type));
    type->kind = kind;
    return type;
}

Type* VoidType(void) {
    return mktype(VOID);
}

Type* IntType(size_t nbits) {
    Type* type = mktype(INT);
    type->value.bits = nbits;
    return type;
}

Type* UIntType(size_t nbits) {
    Type* type = mktype(UINT);
    type->value.bits = nbits;
    return type;
}

Type* ArrayOf(Type* elemtype, size_t count) {
    Type* type = mktype(ARRAY);
    type->value.array.type = elemtype;
    type->value.array.count = count;
    return type;
}

Type* RefTo(Type* type) {
    return NULL;
}

Type* PtrTo(Type* type) {
    return NULL;
}
