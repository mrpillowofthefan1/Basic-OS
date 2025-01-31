[bits 16]

print_string_16:
    pusha
    mov ah, 0x0e
.print_loop:
    mov al, [bx]
    cmp al, 0
    je .done
    int 0x10
    inc bx
    jmp .print_loop
.done:
    popa
    ret

disk_load:
    pusha
    push dx

    mov ah, 0x02
    mov al, dh
    mov cl, 0x02
    mov ch, 0x00
    mov dh, 0x00
    int 0x13
    jc .disk_error

    pop dx
    cmp al, dh
    jne .sectors_error
    popa
    ret

.disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string_16
    jmp $

.sectors_error:
    mov bx, SECTORS_ERROR_MSG
    call print_string_16
    jmp $

DISK_ERROR_MSG: db "Disk error!", 0
SECTORS_ERROR_MSG: db "Sector error!", 0