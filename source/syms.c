#include <sclpl.h>

static Sym* mksym(int flags, char* name, Type* type, Sym* next) {
    Sym* sym = emalloc(sizeof(Sym));
    sym->flags = flags;
    sym->name = name;
    sym->type = type;
    sym->next = next;
    return sym;
}

void sym_add(SymTable* syms, int flags, char* name, Type* type) {
    syms->syms = mksym(flags, name, type, syms->syms);
}

Sym* sym_get(SymTable* syms, char* name) {
    Sym* sym = syms->syms;
    for (; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            return sym;
    return NULL;
}
