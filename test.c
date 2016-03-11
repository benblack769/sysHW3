#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include "cache.h"
#include "helper.h"
#include "replacement.h"
//#include "basic_cache.c"
//#include "hash_cache.c"

#define num_vals (11ULL)

key_type insert_random(cache_t cache,uint32_t size);
key_type keys[] = {"one","two","three","argvar","52","ten thousand","asdasd","asdasdasdasdasd,","!@#!@$!$~`","1","a verryy longgggg stringggggggggg...................................................................................................................................................!"};
val_type vals[num_vals];
size_t val_sizes[] = {0,1,2,26,233,1020,15221,41231,300212,1312312,6362363};
void generate_vals();
void delete_vals();
void test_cache_overflow(uint64_t max_mem);
void test_no_eviction(uint64_t max_mem);
void lru_test(size_t max_mem);
void cache_speed_test(size_t num_elmnts, size_t num_iters, hash_func hash_fn);
uint64_t custom_hash(key_type key);

int main(int argc,char ** argv){
    srand(23);
    generate_vals();
    const size_t num_tests = 5;
    uint64_t test_sizes[] = {10ULL,1000ULL,100000ULL,1000000ULL,10000000ULL};

    for(size_t t = 0; t < num_tests; t++){
        test_no_eviction(test_sizes[t]);
    }
    for(size_t t = 0; t < num_tests; t++){
        test_cache_overflow(test_sizes[t]);
    }
    for(size_t t = 0; t < num_tests-1; t++){
        lru_test(test_sizes[t]);
    }
    for(size_t t = 0; t < num_tests-1; t++){
        clock_t start = clock();
        cache_speed_test(test_sizes[t],test_sizes[t+1],NULL);
        double time = (double)(clock() - start) / CLOCKS_PER_SEC;;
        printf("default hash %f \n",time);
        start = clock();
        cache_speed_test(test_sizes[t],test_sizes[t+1],custom_hash);//tests non-default hash function
        time = (double)(clock() - start) / CLOCKS_PER_SEC;
        printf("custom hash %f \n",time);
        fflush(stdout); 
    }
    
    delete_vals();
    getchar();
    return 0;
}
uint64_t custom_hash(key_type key){
    //tests if the hash is inputed correctly

    //adds the bits of the string together
    size_t tot_size = strlen((char*)(key));
    const size_t out_size = sizeof(uint64_t);
    uint64_t out = 0;
    for(size_t i = 0; i < tot_size / out_size;i++){
        out += ((uint64_t *)(key))[i];
    }
    for(size_t i = 0;i < tot_size%out_size; i++){
        size_t index = i + tot_size - tot_size%out_size;
        out += ((uint64_t)(key[index])) << (i*BITS_IN_BYTE);
    }
    return out;
}

void test_no_eviction(uint64_t max_mem){
    //runs tests of sets and deletes assuming that all added
    //values are still in the cache because the values are all below the maxmem

    cache_t cache = create_cache(max_mem,NULL);
    //should not evict things before memory exceeded test
    uint32_t size_sum = 0;
    size_t num_before_evict = 0;
    for(uint32_t i = 0;i < num_vals && size_sum + val_sizes[i] < max_mem;i++){
        num_before_evict += 1;
        size_sum += val_sizes[i];
    }
    for(size_t i = 0; i < num_before_evict; i++){
        cache_set(cache,keys[i],vals[i],val_sizes[i]);
    }
    if(cache_space_used(cache) != size_sum){
        printf("cache space not what expected, %u expected, %u actual\n",size_sum,cache_space_used(cache));
    }
    //makes sure all values are in the cache
    for(size_t i = 0; i < num_before_evict; i++){
        uint32_t val_size = 0;
        val_type val = cache_get(cache,keys[i],&val_size);
        if(val == NULL){
            printf("value added that did not exceed max_mem is not in cache\n");
        }
        if(val_size != val_sizes[i]){
            printf("size of value not what expected: %u expected, %u actual\n",val_sizes[i],val_size);
        }
        if(memcmp(vals[i],val,val_size)){
            printf("value not what expected, first byte, expected = %u, actual = %u\n",*(uint8_t*)(vals[i]),*(uint8_t*)(val));
        }
    }
    //deletes items in the cache
    const size_t d_i = 4;
    uint32_t d_val_size;
    cache_delete(cache,keys[d_i]);

    if(cache_get(cache,keys[d_i],&d_val_size) != NULL){
        printf("cache_get returned non-null for deleted key\n");
    }
    //checks to see if it crashes if an item is deleted twice
    cache_delete(cache,keys[d_i]);

    for(size_t i = 0; i < num_before_evict; i++){
        if(i == d_i)
            continue;
        uint32_t val_size = 0;
        val_type val = cache_get(cache,keys[i],&val_size);
        if(val == NULL){
            printf("value not deleted is not in cache after another value was deleted\n");
        }
    }

    destroy_cache(cache);
}

uint64_t get_mem_in_cache(cache_t cache){
    //gets memory in the cache without relying on the cache to actually keep track of its own memory
    uint64_t tot_val_size = 0;
    for(size_t i = 0; i < num_vals; i++){
        uint32_t val_size = 0;
        val_type val = cache_get(cache,keys[i],&val_size);
        if(val != NULL){
            tot_val_size += val_size;
        }
    }
    return tot_val_size;
}

void test_cache_overflow(uint64_t max_mem){
    //loads cache full of values and sees if the cache never uses more memory than it really should

    cache_t cache = create_cache(max_mem,NULL);

    for(size_t item_n = 0; item_n < num_vals; item_n++){
        cache_set(cache,keys[item_n],vals[item_n],val_sizes[item_n]);

        uint64_t cache_mem = get_mem_in_cache(cache);
        if(cache_mem != cache_space_used(cache)){
            printf("cache_space_used returned incorrect value\n");
        }
        if(cache_mem > max_mem){
            printf("cache uses more memory than specified\n");
        }
    }
    destroy_cache(cache);
}

void generate_vals(){
    //generates random values for the global "vals" array
    for(size_t v = 0; v < num_vals;v++){
        vals[v] = calloc(val_sizes[v],1);
        uint8_t * valbytes = (uint8_t *)(vals[v]);
        for(size_t i = 0; i < val_sizes[v]; i++){
            valbytes[i] = (uint8_t)(rand());
        }
    }
}
void delete_vals(){
    //deletes random values from the global "vals" array
    for(size_t v = 0; v < num_vals;v++){
        c_delete(&vals[v]);
    }
}

void lru_test(size_t max_mem){
    policy_t policy = create_policy(max_mem);
    const size_t num_elements = max_mem*3;
    size_t * markers = calloc(num_elements,sizeof(size_t));
    pinfo_t * infos = calloc(num_elements,sizeof(pinfo_t));
    //initialize markers
    for(size_t i = 0; i < num_elements; i++){
        markers[i] = i;
    }
   // printf("init\n")
    const uint32_t add_size = 1;//needs to be 1
    for(size_t i = 0; i < max_mem; i++){
        struct id_arr res = ids_to_delete_if_added(policy,add_size);
        if(!res.should_add){
            printf("LRU wants to not add reasonable value\n");
        }
        if(res.size > 0){
            printf("LRU throws out something before max_mem is exceeded\n");
        }
        if(res.data != NULL){
            free(res.data);
        }
        infos[i] = create_info(policy,(user_id_t)(markers[i]),add_size);
    }
    for(size_t i = 0; i < max_mem/2; i++){
        info_gotten(policy,infos[i]);
    }
    struct id_arr res = ids_to_delete_if_added(policy,add_size*max_mem/2);
    if(!res.should_add){
        printf("LRU doesn't want to add a reasonable value\n");
    }
    else if(res.size != max_mem/2){
        printf("LRU wants delete more or less than the correct number of values from the policy\n");
    }
    //checks to see if all elements are indeed the least recently used (in this case, the elements with markers max_mem/2 to max_mem)
    bool * slots = calloc(max_mem/2,sizeof(bool));//initializes all to false
    for(size_t i = 0; i < max_mem/2; i++){
        size_t cur_marker = (size_t)(res.data[i]);
        if(cur_marker >= max_mem || cur_marker < max_mem/2){
            printf("LRU is not an LRU\n");
        }
        else{
            slots[cur_marker - max_mem/2] = true;
            delete_info(policy,infos[cur_marker]);
        }
    }
    for(size_t i = 0; i < max_mem/2; i++){
        if(!slots[i]){
            printf("LRU is not an LRU\n");           
        }
    }
    const size_t bigmarker = 123123123123ULL;
    pinfo_t biginfo = create_info(policy,(user_id_t)(bigmarker),add_size*max_mem/2);
    
    //checks if zero is the next thing to be deleted (as it was the last thing access which was not deleted above)
    struct id_arr fin_res = ids_to_delete_if_added(policy,add_size);
    if(fin_res.size != 1 || fin_res.data[0] != 0){
        printf("LRU is not an LRU\n");
    }

    if(fin_res.data != NULL){
        free(fin_res.data);
    }
    if(res.data != NULL){
        free(res.data);
    }
    delete_policy(policy);
    free(markers);
    free(infos);
    free(slots);
}
uint64_t Rand(){
    return rand();
}
uint64_t long_rand(){
    //rand that has a greater maximum value (xor retains randomness)
    return Rand() ^ (Rand() << 8) ^ (Rand() << 16) ^ (Rand() << 24) ^ (Rand() << 32);
}

void init_keys_to_rand_strs(key_type * keyarr,size_t num_keys){
    //gives each key a string that happens to be createds as a 7 byte integer
    for(size_t i = 0; i < num_keys; i++){
        uint64_t null_term_8_byte_str = long_rand() & 0x00ffffffffffffff; 
        keyarr[i] = calloc(1,sizeof(uint64_t));
        *((uint64_t*)(keyarr[i])) = null_term_8_byte_str;
    }
}
void free_keys(uint8_t ** keyarr,size_t num_keys){
    //helper for cache_speed_test
    for(size_t i = 0; i < num_keys; i++){
        free(keyarr[i]);
    }
}

void cache_speed_test(size_t num_elmnts,size_t num_iters,hash_func hash_fn){
    const uint32_t val_size = sizeof(size_t);
    size_t maxmem = (num_elmnts * val_size) / 2;
    
    key_type * key_arr = calloc(num_elmnts,sizeof(key_type));
    init_keys_to_rand_strs(key_arr,num_elmnts);
    
    uint64_t int_value = 0xcccccccccccccccc;
    val_type my_val = (val_type)(&int_value);
    
    cache_t cache = create_cache(maxmem,hash_fn);
    //randomly sets, gets and deletes items such that items are slowly accumulated
    size_t num_in = 0;
    for(size_t i = 0; i < num_iters; i++){
        size_t ci = i % num_elmnts;
        if(Rand()%10 < 7){
            uint32_t empty = -1;
            cache_get(cache,key_arr[ci],&empty);
        }
        else if(num_in < long_rand() % num_elmnts){
            cache_set(cache,key_arr[ci],my_val,val_size);
            num_in++;
        }
        else{
            cache_delete(cache,key_arr[ci]);
            num_in--;
        }
    }
    destroy_cache(cache);
    free_keys((uint8_t**)(key_arr),num_elmnts);
    free(key_arr);
}
