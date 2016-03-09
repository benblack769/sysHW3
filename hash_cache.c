#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cache.h"
#include "helper.h"
#include "replacement.h"

struct key_val_obj{
    key_t key;
    val_t val;
    uint32_t val_size;
    
    p_info_t policy_info;
};
typedef struct key_val_obj key_val_s;
struct link_obj;
typedef struct link_obj* link_t;
struct link_obj{
    key_val_s data;
    link_t next;
};
void del_link(cache_t cache, link_t * obj);

uint64_t def_hash_fn(key_t key){
    //xors the bits of the string together
    size_t tot_size = strlen((char*)(key));
    const size_t out_size = sizeof(uint64_t);
    uint64_t out = 0;
    for(size_t i = 0; i < tot_size / out_size;i++){
        out ^= ((uint64_t *)(key))[i];
    }
    for(size_t i = 0;i < tot_size % out_size; i++){
        out ^= ((uint64_t)(key[i + tot_size/out_size])) << i;
    }
    return out;
}

size_t hash_location(uint64_t hash_val,size_t table_size){
    //consider converting to multiplicative function
    return hash_val % table_size;
}

struct cache_obj{
    uint64_t maxmem;
    uint64_t mem_used;
    link_t * table;
    size_t table_size;
    size_t slots_used;
    hash_func h_fn;
    policy_t evic_policy;
};
const size_t default_table_size = 199;//medium size prime number

struct user_identifier{//declared in replacement.h
    link_t * linkpp;
};

cache_t create_cache(uint64_t maxmem,hash_func h_fn){
    cache_t n_cache = calloc(1,sizeof(struct cache_obj));

    n_cache->maxmem = maxmem;
    n_cache->mem_used = 0;
    n_cache->table_size = default_table_size;
    n_cache->slots_used = 0;
    n_cache->table = calloc(default_table_size,sizeof(key_val_s));
    n_cache->h_fn = (h_fn == NULL) ? def_hash_fn : h_fn;
    n_cache->evic_policy = create_policy(maxmem);
    return n_cache;
}

link_t * querry_hash(cache_t cache, key_t key){
    size_t hash_loc = hash_location(cache->h_fn(key),cache->table_size);
    link_t * cur_item = &cache->table[hash_loc];
    while(*cur_item != NULL && strcmp((char*)(key),(char*)((*cur_item)->data.key))){
        cur_item = &(*cur_item)->next;
    }
    return cur_item;
}
void assign_to_link(link_t * linkp,key_val_s data){
    if(*linkp == NULL){
        *linkp = calloc(1,sizeof(struct link_obj));
    }
    (*linkp)->data = data;
}
void resize_table(cache_t cache,uint64_t new_size){
    const size_t old_t_size = cache->table_size;
    link_t * old_table = cache->table;

    cache->table = calloc(new_size,sizeof(link_t));
    cache->table_size = new_size;

    for(size_t i = 0; i < old_t_size;i++){
        link_t cur_l = old_table[i];
        while(cur_l != NULL){
            assign_to_link(querry_hash(cache,cur_l->data.key),cur_l->data);
            cur_l = cur_l->next;
        }
    }
    free(old_table);
}
bool take_care_of_eviction_deletions(cache_t cache,uint32_t val_size){
    struct id_arr add_res = ids_to_delete_if_added(cache->evic_policy,val_size);
    bool ret_val = false;
    if(add_res.should_add){
        for(size_t di = 0;di < add_res.size; di++){
            cache_delete(cache,(key_t)(add_res.data[di]));
        }
        if(add_res.data != NULL){
            free(add_res.data);
        }
        ret_val = true;
    }
    return ret_val;
}

void cache_set(cache_t cache, key_t key, val_t val, uint32_t val_size){
    link_t * key_link = querry_hash(cache,key);
    //if the item is in the list, then delete it
    if(*key_link != NULL){
        del_link(cache,key_link);
    }
    //if the policy tell the cache not to add the item, do not add it
    if(!take_care_of_eviction_deletions(cache,val_size)){
        return;
    }
    key_t key_copy = make_copy(key,strlen((char*)key)+1);
    key_val_s new_item = {
        key_copy,
        make_copy(val,val_size),
        val_size,
        create_info(cache->evic_policy,(void*)(key_copy),val_size)};
    
    assign_to_link(key_link,new_item);
    cache->mem_used += val_size;
}
val_t cache_get(cache_t cache, key_t key, uint32_t *val_size){
    link_t hash_l = *querry_hash(cache,key);
    if(hash_l != NULL){
        info_gotten(cache->evic_policy,hash_l->data.policy_info);
        *val_size = hash_l->data.val_size;
        return hash_l->data.val;
    }
    else{
        *val_size = 0;
        return NULL;
    }
}
void del_link(cache_t cache,link_t * obj){
    link_t myobj = *obj;
    if(*obj != NULL){
        *obj = myobj->next;
        free((uint8_t*)myobj->data.key);
        free((void *)myobj->data.val);
        delete_info(cache->evic_policy,myobj->data.policy_info);
        free(myobj);
    }
}
void cache_delete(cache_t cache, key_t key){
    link_t * del_l = querry_hash(cache,key);
    if(*del_l != NULL){
        cache->mem_used -= (*del_l)->data.val_size;
        del_link(cache,del_l);
    }
}
uint64_t cache_space_used(cache_t cache){
    return cache->mem_used;
}
void destroy_cache(cache_t cache){
    for(size_t i = 0; i < cache->table_size; i++){
        while(cache->table[i] != NULL){
            del_link(cache,&cache->table[i]);
        }
    }
    delete_policy(cache->evic_policy);
    free(cache->table);
    free(cache);
}
