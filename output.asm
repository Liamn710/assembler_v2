
section .data
    hello_msg db 'Hello, World!$'

section .text
    global _start

_start:
    PRINT_HELLO
    MOV AH, 4Ch
    INT 21h
