%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90
_NR_dispatch        equ 0

global  dispatch

bits 32
[section .text]

dispatch:
    mov eax, _NR_dispatch
    int INT_VECTOR_SYS_CALL
    ret
