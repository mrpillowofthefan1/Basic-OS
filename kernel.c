#include <stddef.h>
#include <stdint.h>
#pragma once



/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
typedef struct {
    uint16_t low_offset;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

typedef struct {
    // data segment selector
    uint32_t ds;
    // general purpose registers pushed by pusha
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    // pushed by isr procedure
    uint32_t int_no, err_code;
    // pushed by CPU automatically
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_t)(registers_t *);

isr_t interrupt_handlers[256];


idt_gate_t idt[256];
#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].selector = 0x08; // see GDT
    idt[n].always0 = 0;
    // 0x8E = 1  00 0 1  110
    //        P DPL 0 D Type
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
}
char *exception_messages[] = {
    "Division by zero",
    "Debug",
    "Reserved"
};


/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0; // Reset row if it exceeds height
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void isr_handler(registers_t *r) {
    terminal_writestring("received interrupt: ");
    char s[3];
    terminal_writestring(s);
    terminal_writestring(exception_messages[r->int_no]);
}

idt_register_t idt_reg;
#define IDT_ENTRIES 256

void load_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    asm volatile("lidt (%0)" : : "r" (&idt_reg));
}
extern void isr0();

extern void isr1();

extern void isr2();

extern void isr3();

extern void isr4();

extern void isr5();

extern void isr6();

extern void isr7();

extern void isr8();

extern void isr9();

extern void isr10();

extern void isr11();

extern void isr12();

extern void isr13();

extern void isr14();

extern void isr15();

extern void isr16();

extern void isr17();

extern void isr18();

extern void isr19();

extern void isr20();

extern void isr21();

extern void isr22();

extern void isr23();

extern void isr24();

extern void isr25();

extern void isr26();

extern void isr27();

extern void isr28();

extern void isr29();

extern void isr30();

extern void isr31();

/* IRQ definitions */
extern void irq0();

extern void irq1();

extern void irq2();

extern void irq3();

extern void irq4();

extern void irq5();

extern void irq6();

extern void irq7();

extern void irq8();

extern void irq9();

extern void irq10();

extern void irq11();

extern void irq12();

extern void irq13();

extern void irq14();

extern void irq15();

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

void isr_install() {
    set_idt_gate(0, (uint32_t) isr0);
    set_idt_gate(1, (uint32_t) isr1);
    set_idt_gate(2, (uint32_t) isr2);
    set_idt_gate(3, (uint32_t) isr3);
    set_idt_gate(4, (uint32_t) isr4);
    set_idt_gate(5, (uint32_t) isr5);
    set_idt_gate(6, (uint32_t) isr6);
    set_idt_gate(7, (uint32_t) isr7);
    set_idt_gate(8, (uint32_t) isr8);
    set_idt_gate(9, (uint32_t) isr9);
    set_idt_gate(10, (uint32_t) isr10);
    set_idt_gate(11, (uint32_t) isr11);
    set_idt_gate(12, (uint32_t) isr12);
    set_idt_gate(13, (uint32_t) isr13);
    set_idt_gate(14, (uint32_t) isr14);
    set_idt_gate(15, (uint32_t) isr15);
    set_idt_gate(16, (uint32_t) isr16);
    set_idt_gate(17, (uint32_t) isr17);
    set_idt_gate(18, (uint32_t) isr18);
    set_idt_gate(19, (uint32_t) isr19);
    set_idt_gate(20, (uint32_t) isr20);
    set_idt_gate(21, (uint32_t) isr21);
    set_idt_gate(22, (uint32_t) isr22);
    set_idt_gate(23, (uint32_t) isr23);
    set_idt_gate(24, (uint32_t) isr24);
    set_idt_gate(25, (uint32_t) isr25);
    set_idt_gate(26, (uint32_t) isr26);
    set_idt_gate(27, (uint32_t) isr27);
    set_idt_gate(28, (uint32_t) isr28);
    set_idt_gate(29, (uint32_t) isr29);
    set_idt_gate(30, (uint32_t) isr30);
    set_idt_gate(31, (uint32_t) isr31);

    // Remap the PIC
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    // Install the IRQs
    set_idt_gate(32, (uint32_t)irq0);
    set_idt_gate(33, (uint32_t)irq1);
    set_idt_gate(34, (uint32_t)irq2);
    set_idt_gate(35, (uint32_t)irq3);
    set_idt_gate(36, (uint32_t)irq4);
    set_idt_gate(37, (uint32_t)irq5);
    set_idt_gate(38, (uint32_t)irq6);
    set_idt_gate(39, (uint32_t)irq7);
    set_idt_gate(40, (uint32_t)irq8);
    set_idt_gate(41, (uint32_t)irq9);
    set_idt_gate(42, (uint32_t)irq10);
    set_idt_gate(43, (uint32_t)irq11);
    set_idt_gate(44, (uint32_t)irq12);
    set_idt_gate(45, (uint32_t)irq13);
    set_idt_gate(46, (uint32_t)irq14);
    set_idt_gate(47, (uint32_t)irq15);

    load_idt(); // Load with ASM
}
void irq_handler(registers_t *r) {
    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }

    // EOI
    if (r->int_no >= 40) {
        port_byte_out(0xA0, 0x20); /* follower */
    }
    port_byte_out(0x20, 0x20); /* leader */
}





void print_letter(uint8_t scancode) {
    switch (scancode) {
        case 0x0:
            terminal_writestring("ERROR");
            break;
        case 0x1:
            terminal_writestring("ESC");
            break;
        case 0x2:
            terminal_writestring("1");
            break;
        case 0x3:
            terminal_writestring("2");
            break;
        case 0x4:
            terminal_writestring("3");
            break;
        case 0x5:
            terminal_writestring("4");
            break;
        case 0x6:
            terminal_writestring("5");
            break;
        case 0x7:
            terminal_writestring("6");
            break;
        case 0x8:
            terminal_writestring("7");
            break;
        case 0x9:
            terminal_writestring("8");
            break;
        case 0x0A:
            terminal_writestring("9");
            break;
        case 0x0B:
            terminal_writestring("0");
            break;
        case 0x0C:
            terminal_writestring("-");
            break;
        case 0x0D:
            terminal_writestring("+");
            break;
        case 0x0E:
            terminal_writestring("Backspace");
            break;
        case 0x0F:
            terminal_writestring("Tab");
            break;
        case 0x10:
            terminal_writestring("Q");
            break;
        case 0x11:
            terminal_writestring("W");
            break;
        case 0x12:
            terminal_writestring("E");
            break;
        case 0x13:
            terminal_writestring("R");
            break;
        case 0x14:
            terminal_writestring("T");
            break;
        case 0x15:
            terminal_writestring("Y");
            break;
        case 0x16:
            terminal_writestring("U");
            break;
        case 0x17:
            terminal_writestring("I");
            break;
        case 0x18:
            terminal_writestring("O");
            break;
        case 0x19:
            terminal_writestring("P");
            break;
        case 0x1A:
            terminal_writestring("[");
            break;
        case 0x1B:
            terminal_writestring("]");
            break;
        case 0x1C:
            terminal_writestring("ENTER");
            break;
        case 0x1D:
            terminal_writestring("LCtrl");
            break;
        case 0x1E:
            terminal_writestring("A");
            break;
        case 0x1F:
            terminal_writestring("S");
            break;
        case 0x20:
            terminal_writestring("D");
            break;
        case 0x21:
            terminal_writestring("F");
            break;
        case 0x22:
            terminal_writestring("G");
            break;
        case 0x23:
            terminal_writestring("H");
            break;
        case 0x24:
            terminal_writestring("J");
            break;
        case 0x25:
            terminal_writestring("K");
            break;
        case 0x26:
            terminal_writestring("L");
            break;
        case 0x27:
            terminal_writestring(";");
            break;
        case 0x28:
            terminal_writestring("'");
            break;
        case 0x29:
            terminal_writestring("`");
            break;
        case 0x2A:
            terminal_writestring("LShift");
            break;
        case 0x2B:
            terminal_writestring("\\");
            break;
        case 0x2C:
            terminal_writestring("Z");
            break;
        case 0x2D:
            terminal_writestring("X");
            break;
        case 0x2E:
            terminal_writestring("C");
            break;
        case 0x2F:
            terminal_writestring("V");
            break;
        case 0x30:
            terminal_writestring("B");
            break;
        case 0x31:
            terminal_writestring("N");
            break;
        case 0x32:
            terminal_writestring("M");
            break;
        case 0x33:
            terminal_writestring(",");
            break;
        case 0x34:
            terminal_writestring(".");
            break;
        case 0x35:
            terminal_writestring("/");
            break;
        case 0x36:
            terminal_writestring("Rshift");
            break;
        case 0x37:
            terminal_writestring("Keypad *");
            break;
        case 0x38:
            terminal_writestring("LAlt");
            break;
        case 0x39:
            terminal_writestring("Space");
            break;
        default:
            /* 'keuyp' event corresponds to the 'keydown' + 0x80
             * it may still be a scancode we haven't implemented yet, or
             * maybe a control/escape sequence */
            if (scancode <= 0x7f) {
                terminal_writestring("Unknown key down");
            } else if (scancode <= 0x39 + 0x80) {
                terminal_writestring("key up ");
                print_letter(scancode - 0x80);
            } else terminal_writestring("Unknown key up");
            break;
    }
}
static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = port_byte_in(0x60);
    print_letter(scancode);
}
void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}
#define IRQ1 33

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}


void main() {
    terminal_initialize();
    terminal_writestring("Installing interrupt service routines (ISRs).\n");

    terminal_writestring("Enabling external interrupts.\n");

    terminal_writestring("Initializing keyboard (IRQ 1).\n");
}

