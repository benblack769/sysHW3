#pragma once
#include <stdlib.h>
void c_delete(void ** ptr){
    if (*ptr == NULL)
        return;
    free(*ptr);
    ptr = NULL;
}
void * make_copy(const void * buffer,size_t buffsize){
    void * newptr = malloc(buffsize);
    memcpy(newptr,buffer,buffsize);
    return newptr;
}
