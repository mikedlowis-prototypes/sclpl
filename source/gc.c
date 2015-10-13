#include <sclpl.h>

typedef struct obj_t {
    uintptr_t refs;
    destructor_t destructor;
} obj_t;

typedef struct hash_entry_t {
    obj_t* object;
    unsigned int hash;
    struct hash_entry_t* next;
} hash_entry_t;

typedef struct {
    size_t size;
    size_t bkt_count;
    hash_entry_t** buckets;
} hash_t;

static void hash_set(hash_t* hash, hash_entry_t* entry);

/*****************************************************************************/

#define NUM_PRIMES (sizeof(Primes)/sizeof(unsigned int))

static unsigned int Primes[] = {
    5, 13, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593,
    49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469,
    12582917, 25165843, 50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};

static unsigned int num_buckets(unsigned int idx) {
    assert(idx < NUM_PRIMES);
    return Primes[idx];
}

static void hash_init(hash_t* hash)
{
    hash->size      = 0;
    hash->bkt_count = 0;
    hash->buckets   = (hash_entry_t**)calloc(sizeof(hash_entry_t*), num_buckets(hash->bkt_count));
}

static void find_entry(hash_entry_t** parent, hash_entry_t** current, hash_entry_t* entry)
{
    while(*current != NULL) {
        if (((*current)->hash == entry->hash) &&
            ((*current)->object == entry->object))
            break;
        *parent  = *current;
        *current = (*current)->next;
    }
}

static void rehash(hash_t* hash)
{
    unsigned int oldcount, i;
    hash_entry_t** oldbuckets;
    hash_entry_t *node, *entry;
    if ((hash->bkt_count+1) < NUM_PRIMES) {
        oldcount = hash->bkt_count++;
        oldbuckets = hash->buckets;
        hash->buckets = (hash_entry_t**)calloc(sizeof(hash_entry_t*), num_buckets(hash->bkt_count));
        hash->size = 0;
        /* Iterate over all of the old buckets */
        for (i = 0; i < num_buckets(oldcount); i++) {
            node = oldbuckets[i];
            /* re-insert all entries in the bucket into the new bucket table */
            while (node != NULL) {
                entry = node;
                node = entry->next;
                hash_set(hash, entry);
            }
        }
        /* Free the old bucket table */
        free(oldbuckets);
    }
}

static uint64_t hash64(uint64_t key)
{
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

static size_t hash_size(hash_t* hash)
{
    return hash->size;
}

static void hash_set(hash_t* hash, hash_entry_t* entry)
{
    unsigned int index;
    hash_entry_t *parent, *node, *deadite;
    if (hash->size >= num_buckets(hash->bkt_count))
        rehash(hash);
    entry->hash = hash64((uint64_t)(entry->object));
    index   = (entry->hash % num_buckets(hash->bkt_count));
    parent  = NULL;
    node    = hash->buckets[index];
    deadite = NULL;
    find_entry(&parent, &node, entry);
    if ((parent == NULL) && (node == NULL)) {
        hash->buckets[index] = entry;
        entry->next = NULL;
        hash->size++;
    } else if (node == NULL) {
        parent->next = entry;
        entry->next = NULL;
        hash->size++;
    } else if (parent == NULL) {
        deadite = node;
        entry->next = deadite->next;
        hash->buckets[index] = entry;
    } else {
        deadite = node;
        entry->next = deadite->next;
        parent->next = entry;
    }
    if (deadite != NULL)
        free(deadite);
}

static hash_entry_t* hash_del(hash_t* hash, hash_entry_t* entry)
{
    unsigned int index;
    hash_entry_t *parent, *node, *ret = NULL;
    entry->hash = hash64((uint64_t)(entry->object));
    index = (entry->hash % num_buckets(hash->bkt_count));
    parent = NULL;
    node = hash->buckets[index];
    find_entry(&parent, &node, entry);
    if (node != NULL) {
        ret = node;
        node = node->next;
        if (parent != NULL)
            parent->next = node;
        else
            hash->buckets[index] = node;
        hash->size--;
    }
    return ret;
}

/*****************************************************************************/

static bool Shutdown;
static void** Stack_Bottom;
static hash_t Zero_Count_Table;
static hash_t Multi_Ref_Table;
static hash_t Working_Table;

static void gc_mark_region(void** start, void** end) {
    hash_entry_t lookup = {0, 0, 0};
    hash_entry_t* entry;
    for (; start <= end; start++) {
        lookup.object = *start;
        entry = hash_del(&Working_Table, &lookup);
        if (entry != NULL) {
            hash_set(&Zero_Count_Table, entry);
        }
    }
}

static void gc_mark_stack(void) {
    void* stack_top = NULL;
    /* Determine which way the stack grows and scan it for pointers */
    if (&stack_top < Stack_Bottom) {
        gc_mark_region(&stack_top, Stack_Bottom);
    } else {
        gc_mark_region(Stack_Bottom, &stack_top);
    }
}

static void gc_mark(void) {
    jmp_buf env;
    volatile int noinline = 1;
    /* Flush Registers to Stack */
    if (noinline) {
        memset(&env, 0x55, sizeof(jmp_buf));
        (void)setjmp(env);
    }
    /* Avoid Inlining function call so that we scan the registers along with
     * the stack */
    (noinline ? gc_mark_stack : NULL)();
}

static void gc_sweep_table(hash_t* table) {
    unsigned int i;
    /* Delete all the entries in the hash */
    for (i = 0; i < num_buckets(table->bkt_count); i++) {
        hash_entry_t* node = table->buckets[i];
      table->buckets[i] = NULL;
        while (node != NULL) {
            hash_entry_t* deadite = node;
            node = node->next;
            obj_t* obj = ((obj_t*)(deadite->object)-1);
            if (obj->destructor != NULL)
                obj->destructor(deadite->object);
            free(deadite);
        }
    }
    free(table->buckets);
}

void gc_init(void** stack_bottom)
{
    Stack_Bottom = stack_bottom;
    hash_init(&Zero_Count_Table);
    hash_init(&Multi_Ref_Table);
    atexit(gc_deinit);
    Shutdown = false;
}

void gc_deinit(void)
{
    Shutdown = true;
    gc_sweep_table(&Zero_Count_Table);
    gc_sweep_table(&Multi_Ref_Table);
}

void gc_collect(void) {
#ifdef GC_DEBUG_MSGS
    printf("BEFORE - ZCT: %ld MRT: %ld TOT: %ld\n",
        hash_size(&Zero_Count_Table),
        hash_size(&Multi_Ref_Table),
        hash_size(&Zero_Count_Table) + hash_size(&Multi_Ref_Table));
#endif
    Working_Table = Zero_Count_Table;
    hash_init(&Zero_Count_Table);
    gc_mark();
    gc_sweep_table(&Working_Table);
#ifdef GC_DEBUG_MSGS
    printf("AFTER - ZCT: %ld MRT: %ld TOT: %ld\n\n",
        hash_size(&Zero_Count_Table),
        hash_size(&Multi_Ref_Table),
        hash_size(&Zero_Count_Table) + hash_size(&Multi_Ref_Table));
#endif
}

void* gc_alloc(size_t size, destructor_t destructor)
{
    hash_entry_t* entry = (hash_entry_t*)malloc(sizeof(hash_entry_t) + sizeof(obj_t) + size);
    obj_t* p_obj = (obj_t*)(entry+1);
    void* p_data = (void*)(p_obj+1);
    entry->object = p_data;
    p_obj->refs = 0;
    p_obj->destructor = destructor;
    hash_set(&Zero_Count_Table, entry);
    if (hash_size(&Zero_Count_Table) >= 500)
        gc_collect();
    return p_data;
}

void* gc_addref(void* ptr)
{
    hash_entry_t lookup = {0, 0, 0};
    hash_entry_t* entry;
    obj_t* obj = ((obj_t*)ptr-1);
    if ((ptr != NULL) && !Shutdown) {
        obj->refs++;
        if (obj->refs == 1) {
            lookup.object = ptr;
            entry = hash_del(&Zero_Count_Table, &lookup);
            assert(entry != NULL);
            hash_set(&Multi_Ref_Table, entry);
        }
    }
    return ptr;
}

void gc_delref(void* ptr)
{
    hash_entry_t lookup = {0, 0, 0};
    hash_entry_t* entry;
    obj_t* obj = ((obj_t*)ptr-1);
    if ((ptr != NULL) && !Shutdown) {
        obj->refs--;
        if (obj->refs == 0) {
            lookup.object = ptr;
            entry = hash_del(&Multi_Ref_Table, &lookup);
            assert(entry != NULL);
            hash_set(&Zero_Count_Table, entry);
        }
    }
}

void gc_swapref(void** dest, void* newref)
{
    void* oldref = *dest;
    *dest = gc_addref(newref);
    gc_delref(oldref);
}

/*****************************************************************************/

int main(int argc, char** argv)
{
    void* stack_bottom = NULL;
    gc_init(&stack_bottom);
    return user_main(argc, argv);
}

