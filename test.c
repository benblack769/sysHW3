#include "cache.h"
#include <stdio.h>

key_t * insert_random(cache_t cache,uint32_t size);
void insert_random(cache_t cache);
const uint64_t num_vals = 6;
key_t keys[] = {"one","two","three","","1","a very longgggg stringggggggggg............................."};
val_t vals[num_vals];
size_t val_sizes[] = {0,1,2,26,1020,45221};
void gen_vals();
int main(int argc,char ** argv){


    return 0;
}

void gen_vals(){
    for(int i = 0; i < num_vals;i++){
        vals[i] = calloc();
    }
}
