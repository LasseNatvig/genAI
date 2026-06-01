// assembler.s

.global asmAdd
asmAdd:
    ADD R0, R0, R1
    BX LR

// Subtraction routine for armV7
// Input: R0 = minuend, R1 = subtrahend
// Output: R0 = result (minuend - subtrahend)
.global asmSub
asmSub:
    // Perform subtraction: R0 = R0 - R1
    SUB R0, R0, R1
    // Return to caller
    BX LR

// Multiplication routine by repeated addition for armV7
// Input: R0 = multiplicand, R1 = multiplier
// Output: R0 = result (multiplicand * multiplier)
// Implemented using repeated addition algorithm
//
.global asmMul
asmMul:
    // Set up result register (R2) and counter (R3)
    MOV R2, #0          // Initialize result to 0
    MOV R3, #0          // Initialize counter to 0

    // Check if multiplier is zero (early exit)
    CMP R1, #0
    BEQ mul_done

    // Positive multiplier case
mul_loop:
    // Add multiplicand to result
    ADD R2, R2, R0
    // Increment counter
    ADD R3, R3, #1
    // Compare counter with multiplier
    CMP R3, R1
    BLT mul_loop        // Continue if counter < multiplier

mul_done:
    // Store result in R0 for return
    MOV R0, R2
    // Return to caller
    BX LR
