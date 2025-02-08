#include "keyboard.h"
#include "ports.h"
#include "isr.h"
#include "display.h"

void handle_backspace() {
    int offset = get_cursor();

    if(offset > 0) {
        offset -= 2;

        set_char_at_video_memory(' ', offset);

        set_cursor(offset);
    }
}

void print_letter(uint8_t scancode) {
    switch (scancode) {
        case 0x0:
            print_string("ERROR");
            break;
        case 0x1:
            print_string("ESC");
            break;
        case 0x2:
            print_string("1");
            break;
        case 0x3:
            print_string("2");
            break;
        case 0x4:
            print_string("3");
            break;
        case 0x5:
            print_string("4");
            break;
        case 0x6:
            print_string("5");
            break;
        case 0x7:
            print_string("6");
            break;
        case 0x8:
            print_string("7");
            break;
        case 0x9:
            print_string("8");
            break;
        case 0x0A:
            print_string("9");
            break;
        case 0x0B:
            print_string("0");
            break;
        case 0x0C:
            print_string("-");
            break;
        case 0x0D:
            print_string("+");
            break;
        case 0x0E:
            handle_backspace();
            break;
        case 0x0F:
            print_string("    ");
            break;
        case 0x10:
            print_string("Q");
            break;
        case 0x11:
            print_string("W");
            break;
        case 0x12:
            print_string("E");
            break;
        case 0x13:
            print_string("R");
            break;
        case 0x14:
            print_string("T");
            break;
        case 0x15:
            print_string("Y");
            break;
        case 0x16:
            print_string("U");
            break;
        case 0x17:
            print_string("I");
            break;
        case 0x18:
            print_string("O");
            break;
        case 0x19:
            print_string("P");
            break;
        case 0x1A:
            print_string("[");
            break;
        case 0x1B:
            print_string("]");
            break;
        case 0x1C:
            print_nl();
            break;
        case 0x1D:
            print_string("LCtrl");
            break;
        case 0x1E:
            print_string("A");
            break;
        case 0x1F:
            print_string("S");
            break;
        case 0x20:
            print_string("D");
            break;
        case 0x21:
            print_string("F");
            break;
        case 0x22:
            print_string("G");
            break;
        case 0x23:
            print_string("H");
            break;
        case 0x24:
            print_string("J");
            break;
        case 0x25:
            print_string("K");
            break;
        case 0x26:
            print_string("L");
            break;
        case 0x27:
            print_string(";");
            break;
        case 0x28:
            print_string("'");
            break;
        case 0x29:
            print_string("`");
            break;
        case 0x2A:
            print_string("LShift");
            break;
        case 0x2B:
            print_string("\\");
            break;
        case 0x2C:
            print_string("Z");
            break;
        case 0x2D:
            print_string("X");
            break;
        case 0x2E:
            print_string("C");
            break;
        case 0x2F:
            print_string("V");
            break;
        case 0x30:
            print_string("B");
            break;
        case 0x31:
            print_string("N");
            break;
        case 0x32:
            print_string("M");
            break;
        case 0x33:
            print_string(",");
            break;
        case 0x34:
            print_string(".");
            break;
        case 0x35:
            print_string("/");
            break;
        case 0x36:
            print_string("Rshift");
            break;
        case 0x37:
            print_string("Keypad *");
            break;
        case 0x38:
            print_string("LAlt");
            break;
        case 0x39:
            print_string(" ");
            break;
        default:
            break;
    }
}

#include "ports.h"
#include "display.h"
#include <stdint.h>

#define BACKSPACE 0x0E
#define ENTER 0x1C

// US keyboard layout (scancode set 1)
char scancode_table[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // Backspace
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', // Enter
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0 // Spacebar
};

// Function to get the pressed key
char get_key_pressed() {
    uint8_t scancode = port_byte_in(0x60); // Read from keyboard port

    if (scancode & 0x80) {
        // Ignore key releases (MSB set)
        return 0;
    }

    return scancode_table[scancode]; // Convert scancode to ASCII
}


static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = port_byte_in(0x60);
    print_letter(scancode);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);

}
