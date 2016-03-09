#include <stdlib.h>
#include "cache.h"
#include "link.h"
/*
struct cache_obj{
    uint64_t maxmem;
    uint64_t mem_used;
    link * LL;
};
cache_t create_cache(uint64_t maxmem,hash_func h_fn){
    cache_t n_cache = calloc(1,sizeof(struct cache_obj));

    n_cache->maxmem = maxmem;
    n_cache->mem_used = 0;
    n_cache->LL = NULL;
    return n_cache;
}
void cache_set(cache_t cache, key_t key, val_t val, uint32_t val_size){
    add_link(&cache->LL,key,val,val_size);
    cache->mem_used += val_size;
}
val_t cache_get(cache_t cache, key_t key, uint32_t *val_size){
    link ** ploc = get_linkpp(&cache->LL,key);
    if(ploc == NULL){
        *val_size = 0;
        return NULL;
    }
    else{
        struct val_item val = (*ploc)->val;
        *val_size = val.val_size;
        return val.value;
    }
}
void cache_delete(cache_t cache, key_t key){
    link ** ploc = get_linkpp(&cache->LL,key);
    if (ploc != NULL){
        cache->mem_used -= (*ploc)->val.val_size;
        delete_link(ploc);
    }
}
uint64_t cache_space_used(cache_t cache){
    return cache->mem_used;
}
void destroy_cache(cache_t cache){
    delete_list(&cache->LL);
    c_delete(&cache);
}
*/
