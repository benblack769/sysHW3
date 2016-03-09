#pragma once
#include <string.h>
#include "cache.h"
#include "helper.h"

struct linkstruct{
    uint8_t * key;
    struct val_item val;
    struct linkstruct * next;
};
typedef struct linkstruct link;
void add_link(link ** list,key_type key,val_type val,uint32_t val_size){
    link * oldlink = *list;
    *list = calloc(1,sizeof(link));
    link * curlink = *list;

    curlink->key = make_copy(key,strlen((char *)(key))+1);
    curlink->val.value = make_copy(val,val_size);
    curlink->val.val_size = val_size;
    curlink->next = oldlink;
}
void delete_link(link ** list){
    if(*list == NULL)
        return;
    link * curl = *list;
    link * newl = curl->next;
    free(curl->key);
    free(curl->val.value);
    free(curl);
    *list = newl;
}
link ** get_linkpp(link ** list,key_type key){
    while(*list != NULL){
        if (strcmp((char*)(*list)->key,(char*)(key)) == 0)
            return list;
        list = &(*list)->next;
    }
    return NULL;
}
void delete_list(link ** list){
    while(*list != NULL)
        delete_link(list);
}
