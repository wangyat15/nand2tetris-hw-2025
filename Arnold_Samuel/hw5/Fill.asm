section .bss

section .data

section .text
_start:
    mov eax, 0

loop:
    in al, 0x60
    test al, al
    jz clear_screen
    call fill_black
    jmp loop

clear_screen:
    call fill_white
    jmp loop

fill_black:
    mov di, 0xB8000
    mov cx, 2000
    mov al, 0x20
    mov ah, 0x0F
.fill_black_loop:
    mov [di], ax
    add di, 2
    loop .fill_black_loop
    ret

fill_white:
    mov di, 0xB8000
    mov cx, 2000
    mov al, 0x20
    mov ah, 0xF0
.fill_white_loop:
    mov [di], ax
    add di, 2
    loop .fill_white_loop
    ret