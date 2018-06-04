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

bool types_equal(Type* type1, Type* type2) {
    if (type1->kind != type2->kind) return false;
    switch (type1->kind) {
        case ARRAY:
            return (types_equal(type1->value.array.type, type2->value.array.type) &&
                    (type1->value.array.count == type1->value.array.count));
        default:
            return true;
    }
}

