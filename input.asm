macr PRINT_HELLO
    MOV AX, 0900h
    MOV DX, OFFSET hello_msg
    INT 21h
endmacr
macr PRINT_SOMETHING
    second macro
endmacr
section .text
    global _start
_start:
    PRINT_HELLO
    MOV AH, 4Ch
    INT 21h
    PRINT_SOMETHING