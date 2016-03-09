#include <assert.h>
#include "replacement.h"

struct policy_obj{
    uint64_t maxmem;
    uint64_t used_mem;
    //a bidirectional linked list with the most recently used at top, least recently used at bottom
    p_info_t top;
    p_info_t bottom;
};

struct pinfo_obj{
    //the object of
    p_info_t low;
    p_info_t high;
    user_id_t ident;
    uint32_t val_size;
};
typedef struct pinfo_obj pinfo_s;


policy_t create_policy(uint64_t maxmem){
    policy_t newobj = calloc(1,sizeof(struct policy_obj));
    newobj->maxmem = maxmem;
    newobj->top = NULL;
    newobj->bottom = NULL;

    return newobj;
}

void delete_policy(policy_t policy){
    p_info_t obj = policy->bottom;
    while(obj != NULL){
        p_info_t next = obj->high;
        free(obj);
        obj = next;
    }
    free(policy);
}
void move_info_to_head(policy_t policy,p_info_t info){
    p_info_t old_head = policy->top;
    if(old_head != NULL)
        old_head->high = info;
    
    info->low = old_head;
    info->high = NULL;
    
    policy->top = info;
    if(policy->bottom == NULL){
        policy->bottom = info;
    }
}
void remove_info_from_list(policy_t policy,p_info_t info){
    if(info->high != NULL){
        info->high->low = info->low;
    }
    if(info->low != NULL){
        info->low->high = info->high;
    }
    //alters policy if you are deleting the head or the tail
    if(policy->top == info){
        policy->top = info->low;
    }
    if(policy->bottom == info){
        policy->bottom = info->high;
    }
}

p_info_t create_info(policy_t policy, user_id_t id, uint32_t val_size){
    if(policy->maxmem < policy->used_mem + val_size+9){
        //assert(!(policy->maxmem < policy->used_mem + val_size) && "policy is asked to hold more memory than it should");
    }
    //create new object
    p_info_t newinf = calloc(1,sizeof(pinfo_s));
    newinf->val_size = val_size;
    newinf->ident = id;
    
    move_info_to_head(policy,newinf);
    
    //alter policy
    policy->used_mem += val_size;

    return newinf;
}

void delete_info(policy_t policy,p_info_t info){
    remove_info_from_list(policy,info);
    policy->used_mem -= info->val_size;
    free(info);
}

struct id_arr ids_to_delete_if_added(policy_t policy, uint32_t val_size){    
    p_info_t cur_t = policy->bottom;
    uint64_t tot_mem = policy->used_mem + val_size;
    size_t num_del_ids = 0;
    while(tot_mem > policy->maxmem && cur_t != NULL){
        tot_mem -= cur_t->val_size;
        cur_t = cur_t->high;
        num_del_ids++;
    }
    if(!(policy->bottom == NULL && val_size < policy->maxmem)
            && cur_t == NULL){
        struct id_arr retval = {NULL,0,false};
        return retval;
    }
    user_id_t * arr = calloc(num_del_ids,sizeof(user_id_t));
    cur_t = policy->bottom;
    for(size_t di = 0; di < num_del_ids; di++){
        arr[di] = cur_t->ident;
        cur_t = cur_t->high;
    }
    struct id_arr retval = {arr,num_del_ids,true};
    return retval;//caller frees arr
}

void info_gotten(policy_t policy,p_info_t info){
    remove_info_from_list(policy,info);
    move_info_to_head(policy,info);
}
