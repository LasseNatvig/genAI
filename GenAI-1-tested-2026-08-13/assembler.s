// assembler.s

.global asmAdd
asmAdd:
    ADD R0, R0, R1
    BX LR

.global asmSub
asmSub:
    SUB R0, R0, R1
    BX LR

.global asmMultiply
asmMultiply:
    MOV R2, R0      @ Store the multiplicand in R2
    MOV R3, R1      @ Store the multiplier in R3
    MOV R0, #0      @ Initialize result (R0) to 0

    CMP R3, #0      @ Check if multiplier is 0
    BEQ multiply_done

    CMP R3, #0      @ Check if multiplier is negative
    BLT multiply_negative

multiply_loop:
    ADD R0, R0, R2  @ Add multiplicand to result
    SUB R3, R3, #1  @ Decrement multiplier
    CMP R3, #0      @ Check if we're done
    BGT multiply_loop
    B multiply_done

multiply_negative:
    SUB R0, R0, R2  @ Subtract multiplicand from result (since multiplier is negative)
    ADD R3, R3, #1  @ Increment multiplier (towards 0)
    CMP R3, #0      @ Check if we're done
    BLT multiply_negative

multiply_done:
    BX LR
.end
