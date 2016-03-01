#pragma once
#include <string.h>
#include "cache.h"
#include "helper.h"
struct val_item{
    void * value;
    uint32_t val_size;
};
struct linkstruct{
    uint8_t * key;
    struct val_item val;
    struct linkstruct * next;
};
typedef struct linkstruct link;
void add_link(link ** list,key_t key,val_t val,uint32_t val_size){
    while(*list != NULL)
        list = &((*list)->next);
    *list = calloc(1,sizeof(link));
    link * curlink = *list;

    curlink->key = make_copy(key,strlen(key));
    curlink->val.value = make_copy(val,val_size);
}
void delete_link(link ** list){
    if(*list == NULL)
        return;
    link * curl = *list;
    link * newl = curl->next;
    c_delete(&curl->key);
    c_delete(&curl->val.value);
    c_delete(curl);
    *list = newl;
}
link ** get_linkpp(link ** list,key_t key){
    while(*list != NULL){
        if (strcmp((*list)->key,key))
            return list;
        list = &(*list)->next;
    }
    return NULL;
}
void delete_list(link ** list){
    while(*list != NULL)
        delete_link(list);
}
