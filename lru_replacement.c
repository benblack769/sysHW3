#include "replacement.h"

struct policy_obj{
    //a bidirectional linked list with the most recently used at it's head, least recently used at it's tail

};

struct pinfo_obj{

};


policy_t create_policy(uint64_t maxmem){

}

void delete_policy(policy_t policy){

}

p_info_t create_info(policy_t policy,key_t key, uint32_t val_size){

}

void delete_info(policy_t policy,p_info_t info){

}

struct key_arr keys_to_delete_if_added(policy_t policy, uint32_t val_size){

}

void key_gotten(policy_t policy,p_info_t info){

}
