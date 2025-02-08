#pragma once
#include "util.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct dynamic_mem_node {
    uint32_t size;
    bool used;
    struct dynamic_mem_node *next;
    struct dynamic_mem_node *prev;
} dynamic_mem_node_t;

#define NULL_POINTER ((void*)0)
#define DYNAMIC_MEM_TOTAL_SIZE 4*1024
#define DYNAMIC_MEM_NODE_SIZE sizeof(dynamic_mem_node_t) // 16

#pragma once

void memory_copy(uint8_t *source, uint8_t *dest, uint32_t nbytes);

void init_dynamic_mem();

void print_dynamic_node_size();

void print_dynamic_mem();

void *mem_alloc(size_t size);

void mem_free(void *p);