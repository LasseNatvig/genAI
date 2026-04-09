.syntax unified
.cpu cortex-a8

.section .data

a:
    .word 1
    .word 2
    .word 3
    .word 4

.text
.align 2
.global multiply_repeated_addition
.type multiply_repeated_addition, %function

multiply_repeated_addition:
    push {r4, lr}

    ldr r2, [r0]          @ r2 = multiplicand
    ldr r1, [r1]          @ r1 = multiplier
    mov r0, #0            @ r0 = accumulated result
    mov r4, #0            @ r4 = 1 if the final result must be negative

    cmp r2, #0
    bge check_second
    rsb r2, r2, #0
    eor r4, r4, #1

check_second:
    cmp r1, #0
    bge multiply_loop_check
    rsb r1, r1, #0
    eor r4, r4, #1

multiply_loop_check:
    cmp r1, #0
    beq apply_sign

multiply_loop:
    add r0, r0, r2
    subs r1, r1, #1
    bne multiply_loop

apply_sign:
    cmp r4, #0
    beq done
    rsb r0, r0, #0

done:
    pop {r4, lr}
    bx lr
