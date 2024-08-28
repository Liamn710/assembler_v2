section .text
    global _start
_start:
MOV AX, 0900h
MOV DX, OFFSET hello_msg
INT 21h
    MOV AH, 4Ch
    INT 21h
second macro
