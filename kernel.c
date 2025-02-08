#include "display.h"
#include "isr.h"
#include "keyboard.h"
#include "ports.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_PROCESSES 2
#define TOTAL_TICKS 1000
#define ENABLE_TESTS 1  // Set to 0 to disable tests

typedef struct {
    int weights[4];
    int threshold;
} NeuralNetwork;

typedef struct {
    int pid;
    uint32_t cpu_time;
    uint32_t wait_time;
    int priority;
    bool active;
    uint32_t cpu_ticks;
} Process;

Process processes[MAX_PROCESSES];
NeuralNetwork nn;
volatile uint32_t system_ticks = 0;

// Timer and Interrupt Handlers
void timer_callback(registers_t *regs) {
    system_ticks++;
    port_byte_out(0x20, 0x20);
}

void isr6_handler(registers_t *regs) {
    print_string("\nInvalid Opcode!");
    asm volatile("cli; hlt");
}

void init_timer() {
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);

    register_interrupt_handler(32, &timer_callback);
    uint32_t divisor = 1193180 / 100;
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xFF);
    port_byte_out(0x40, divisor >> 8);
}

// Neural Network
void init_neural_network() {
    nn.weights[0] = 1;   // CPU time
    nn.weights[1] = -2;  // Wait time
    nn.weights[2] = 3;   // Priority
    nn.weights[3] = 1;   // Memory
    nn.threshold = 25;
}

int calculate_activation(Process *p) {
    int features[4] = {
        p->cpu_time / 200,
        p->wait_time / 100,
        p->priority * 2,
        1
    };

    int activation = 0;
    for(int i = 0; i < 4; i++)
        activation += features[i] * nn.weights[i];
    return activation;
}

int nn_predict(int activation) {
    return activation > nn.threshold ? 1 : 0;
}

// CPU Analysis
void analyze_cpu_usage() {
    print_string("\nCPU Usage Report:\n");
    int total_ticks = 0;

    for(int i = 0; i < MAX_PROCESSES; i++)
        total_ticks += processes[i].cpu_ticks;

    for(int i = 0; i < MAX_PROCESSES; i++) {
        int usage = (processes[i].cpu_ticks * 100) / (total_ticks ? total_ticks : 1);
        int activation = calculate_activation(&processes[i]);

        print_string("Process ");
        print_int(processes[i].pid);
        print_string(": ");
        print_int(usage);
        print_string("% [");
        print_string(nn_predict(activation) ? "OPTIMAL" : "SUBOPTIMAL");
        print_string("] (Score: ");
        print_int(activation);
        print_string(")\n");
    }
}

// Test Functions
void test_nn_functions() {
    print_string("\nRunning AI Scheduler Tests...\n");

    Process test_p1 = {1, 400, 50, 4, true, 0};
    Process test_p2 = {2, 100, 200, 5, true, 0};

    int act1 = calculate_activation(&test_p1);
    int act2 = calculate_activation(&test_p2);

    print_string("Test 1 - Activation: ");
    print_int(act1);
    print_string(" | Normal: ~20\n");

    print_string("Test 2 - Activation: ");
    print_int(act2);
    print_string(" | Normal: ~10\n");

    print_string("Test 1 - Prediction: ");
    print_string(nn_predict(act1) ? "OPTIMAL\n" : "SUBOPTIMAL\n");

    print_string("Test 2 - Prediction: ");
    print_string(nn_predict(act2) ? "OPTIMAL\n" : "SUBOPTIMAL\n");

    print_string("AI Scheduler Tests Completed!\n");
}

int main() {
    clear_screen();
    isr_install();
    register_interrupt_handler(6, &isr6_handler);
    init_timer();
    init_neural_network();
    init_keyboard();
    asm volatile("sti");

#if ENABLE_TESTS
    test_nn_functions();
#endif

    // Initialize processes
    processes[0] = (Process){1, 300, 150, 4, true, 0};
    processes[1] = (Process){2, 200, 100, 5, true, 0};

    uint32_t start_ticks = system_ticks;
    int last_selected = -1;
    int consecutive = 0;

    while(system_ticks < start_ticks + TOTAL_TICKS) {
        int max_activation = -1000;
        int selected = 0;

        for(int i = 0; i < MAX_PROCESSES; i++) {
            if(!processes[i].active) continue;

            int activation = calculate_activation(&processes[i]);
            if(activation > max_activation) {
                max_activation = activation;
                selected = i;
            }
        }

        if(selected == last_selected) {
            if(++consecutive >= 2) {
                selected = (selected + 1) % MAX_PROCESSES;
                consecutive = 0;
            }
        } else {
            consecutive = 0;
        }

        processes[selected].cpu_ticks++;
        last_selected = selected;
    }

    analyze_cpu_usage();
    while(1) asm volatile("hlt");
    return 0;
}
