.syntax unified
    .cpu cortex-m0
    .thumb

    /* 1. The Vector Table (Required for Cortex-M) */
    .section ".vector_table", "a"
    .word __stack_end    /* Initial Stack Pointer */
    .word _start         /* Reset Vector (Start Address) */

    /* 2. The Startup Code */
    .section .text
    .thumb_func
    .global _start
_start:
    /* Zero out registers (Good practice) */
    movs r0, #0
    movs r1, #0
    movs r2, #0
    movs r3, #0

    /* Initialize Stack Pointer */
    ldr r0, =__stack_end
    mov sp, r0

    /* Jump to C++ main */
    bl main

    /* If main returns, jump to exit */
    bl exit

    /* Infinite loop safety net */
hang:
    b hang