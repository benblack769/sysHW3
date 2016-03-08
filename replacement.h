#pragma once
#include <stdbool.h>
#include "helper.h"
struct policy_obj;
struct pinfo_obj;
typedef struct policy_obj* policy_t;
typedef struct pinfo_obj* p_info_t;

policy_t create_policy(uint64_t maxmem);
void delete_policy(policy_t policy);

struct key_arr{
    key_t * data;
    size_t size;
    bool should_add;
};
//adds the info to the policy
p_info_t create_info(policy_t policy,key_t key, uint32_t val_size);
//also removes from the policy
void delete_info(policy_t policy,p_info_t info);

//key_t is an array that is created of all the keys that need to be deleted for the value to be added
//if the should_add value is non-zero, then, the key should not be added to the cache at all (for instance, val_size > max_mem)
struct key_arr keys_to_delete_if_added(policy_t policy, uint32_t val_size);

//gives policy information about the cache.
void key_gotten(policy_t policy,p_info_t info);
