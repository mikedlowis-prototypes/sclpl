#include <sclpl.h>

Sym* mksym(char* name, Type* type, bool is_typedef, Sym* next) {
    Sym* sym = emalloc(sizeof(Sym));
    sym->name = name;
    sym->type = type;
    sym->is_typedef = is_typedef;
    sym->next = next;
    return sym;
}

void sym_adddef(SymTable* syms, char* name, Type* type) {
    syms->syms = mksym(name, type, false, syms->syms);
}

void sym_addtype(SymTable* syms, char* name, Type* type) {
    syms->syms = mksym(name, type, true, syms->syms);
}

Sym* sym_get(SymTable* syms, char* name) {
    Sym* sym = syms->syms;
    for (; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            return sym;
    return NULL;
}
