#include "memory.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// 🧠 Neural Network Parameters
#define INPUT_NODES 5
#define HIDDEN_NODES 10
#define OUTPUT_NODES 1
#define LEARNING_RATE 0.1

// 📦 Memory Management Variables
static uint8_t dynamic_mem_area[DYNAMIC_MEM_TOTAL_SIZE];
static dynamic_mem_node_t *dynamic_mem_start;

// 🔢 Neural Network Weights
float weights_input_hidden[INPUT_NODES][HIDDEN_NODES];
float weights_hidden_output[HIDDEN_NODES][OUTPUT_NODES];

// 🧠 AI Memory Tracking Buffer
size_t recent_allocations[INPUT_NODES] = {128, 256, 512, 1024, 2048};

// 📈 Activation Function (Sigmoid)
float sigmoid(float x) {
    return 1.0 / (1.0 + exp(-x));
}

// 🔮 Predict the Next Memory Allocation Size
size_t predict_next_allocation_size() {
    float hidden_layer[HIDDEN_NODES] = {0};
    float output_layer[OUTPUT_NODES] = {0};

    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < INPUT_NODES; j++) {
            hidden_layer[i] += recent_allocations[j] * weights_input_hidden[j][i];
        }
        hidden_layer[i] = sigmoid(hidden_layer[i]);
    }

    for (int i = 0; i < OUTPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            output_layer[i] += hidden_layer[j] * weights_hidden_output[j][i];
        }
        output_layer[i] = sigmoid(output_layer[i]) * 4096;  // Scale output
    }

    return (size_t)output_layer[0];
}

// 🎯 Train the AI Model
void train_fnn(size_t actual_size) {
    float hidden_layer[HIDDEN_NODES] = {0};
    float output_layer[OUTPUT_NODES] = {0};
    float output_error[OUTPUT_NODES] = {0};
    float hidden_error[HIDDEN_NODES] = {0};

    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < INPUT_NODES; j++) {
            hidden_layer[i] += recent_allocations[j] * weights_input_hidden[j][i];
        }
        hidden_layer[i] = sigmoid(hidden_layer[i]);
    }

    for (int i = 0; i < OUTPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            output_layer[i] += hidden_layer[j] * weights_hidden_output[j][i];
        }
        output_layer[i] = sigmoid(output_layer[i]) * 4096;
    }

    output_error[0] = (float)actual_size - output_layer[0];

    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            weights_hidden_output[i][j] += LEARNING_RATE * output_error[j] * hidden_layer[i];
        }
    }

    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            hidden_error[j] += output_error[0] * weights_hidden_output[j][0];
            weights_input_hidden[i][j] += LEARNING_RATE * hidden_error[j] * recent_allocations[i];
        }
    }
}

dynamic_mem_node_t *find_best_mem_block(dynamic_mem_node_t *dynamic_mem, size_t size) {
    dynamic_mem_node_t *best_mem_block = NULL_POINTER;
    uint32_t best_mem_block_size = DYNAMIC_MEM_TOTAL_SIZE + 1;

    dynamic_mem_node_t *current_mem_block = dynamic_mem;
    while (current_mem_block) {
        if (!current_mem_block->used &&
            current_mem_block->size >= (size + DYNAMIC_MEM_NODE_SIZE) &&
            current_mem_block->size < best_mem_block_size) {
            best_mem_block = current_mem_block;
            best_mem_block_size = current_mem_block->size;
            }
        current_mem_block = current_mem_block->next;
    }
    return best_mem_block;
}


// 🏗️ Initialize Memory Manager and AI Model
void init_dynamic_mem() {
    dynamic_mem_start = (dynamic_mem_node_t *)dynamic_mem_area;
    dynamic_mem_start->size = DYNAMIC_MEM_TOTAL_SIZE - DYNAMIC_MEM_NODE_SIZE;
    dynamic_mem_start->next = NULL_POINTER;
    dynamic_mem_start->prev = NULL_POINTER;

    // 🔄 Initialize Neural Network Weights
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            weights_input_hidden[i][j] = (float)(rand() % 100) / 1000.0;
        }
    }

    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            weights_hidden_output[i][j] = (float)(rand() % 100) / 1000.0;
        }
    }
}

// 📦 AI-Powered Memory Allocation
void *mem_alloc(size_t size) {
    size_t predicted_size = predict_next_allocation_size();

    if (abs((int)size - (int)predicted_size) < 128) {
        size = predicted_size;
    }

    dynamic_mem_node_t *best_mem_block =
        (dynamic_mem_node_t *)find_best_mem_block(dynamic_mem_start, size);

    if (best_mem_block != NULL_POINTER) {
        best_mem_block->size -= size + DYNAMIC_MEM_NODE_SIZE;
        dynamic_mem_node_t *mem_node_allocate =
            (dynamic_mem_node_t *)(((uint8_t *)best_mem_block) + DYNAMIC_MEM_NODE_SIZE + best_mem_block->size);

        mem_node_allocate->size = size;
        mem_node_allocate->used = true;
        mem_node_allocate->next = best_mem_block->next;
        mem_node_allocate->prev = best_mem_block;

        if (best_mem_block->next != NULL_POINTER) {
            best_mem_block->next->prev = mem_node_allocate;
        }
        best_mem_block->next = mem_node_allocate;

        memmove(&recent_allocations[1], &recent_allocations[0], (INPUT_NODES - 1) * sizeof(size_t));
        recent_allocations[0] = size;

        train_fnn(size);

        return (void *)((uint8_t *)mem_node_allocate + DYNAMIC_MEM_NODE_SIZE);
    }

    return NULL_POINTER;
}

// ♻️ Memory Deallocation with AI-Based Merging
void mem_free(void *p) {
    if (p == NULL_POINTER) return;

    dynamic_mem_node_t *current_mem_node =
        (dynamic_mem_node_t *)((uint8_t *)p - DYNAMIC_MEM_NODE_SIZE);

    if (current_mem_node == NULL_POINTER) return;

    current_mem_node->used = false;

    dynamic_mem_node_t *next_mem_node = current_mem_node->next;
    if (next_mem_node != NULL_POINTER && !next_mem_node->used) {
        current_mem_node->size += next_mem_node->size + DYNAMIC_MEM_NODE_SIZE;
        current_mem_node->next = next_mem_node->next;
        if (next_mem_node->next != NULL_POINTER) {
            next_mem_node->next->prev = current_mem_node;
        }
    }

    dynamic_mem_node_t *prev_mem_node = current_mem_node->prev;
    if (prev_mem_node != NULL_POINTER && !prev_mem_node->used) {
        prev_mem_node->size += current_mem_node->size + DYNAMIC_MEM_NODE_SIZE;
        prev_mem_node->next = current_mem_node->next;
        if (current_mem_node->next != NULL_POINTER) {
            current_mem_node->next->prev = prev_mem_node;
        }
    }
}
