[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000
BOOT_DRIVE    db 0

; Set VGA text mode (80x25)
mov ax, 0x0003
int 0x10

; Initialize segments and stack
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

mov [BOOT_DRIVE], dl

call load_kernel
call switch_to_32bit

jmp $

%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

[bits 16]
load_kernel:
    mov ax, 0x0000
    mov es, ax
    mov bx, KERNEL_OFFSET ; ES:BX = 0x0000:0x1000
    mov dh, 15            ; Load 15 sectors (~7.5KB)
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_32BIT:
    call KERNEL_OFFSET ; Jump to kernel
    jmp $

times 510-($-$$) db 0
dw 0xaa55