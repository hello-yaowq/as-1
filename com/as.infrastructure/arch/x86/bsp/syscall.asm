%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90
_NR_dispatch        equ 0

global  dispatch

bits 32
[section .text]

; void dispatch(int cmd)
dispatch:
    mov eax, _NR_dispatch
    mov ebx, [esp + 4]
    int INT_VECTOR_SYS_CALL
    ret
