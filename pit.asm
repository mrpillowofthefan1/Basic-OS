global pit_init
extern outb

pit_init:
    mov al, 0x36          ; Command word: Mode 3 (Square Wave)
    out 0x43, al

    mov ax, 1193          ; Frequency = 100 Hz (10ms interval)
    out 0x40, al          ; Low byte
    mov al, ah
    out 0x40, al          ; High byte

    ret