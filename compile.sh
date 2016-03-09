#!/bin/bash

gcc -O3 -std=c99 -o test test.c hash_cache.c lru_replacement.c helper.c
