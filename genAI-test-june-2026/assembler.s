// assembler.s

.global asmAdd 
asmAdd:
    ADD R0, R0, R1
    BX LR

.global asmRoutine
asmRoutine:
    PUSH {R7}
    AND R7, R0, R1
    ADD R7, R7, #2
    MOV R0, R7
    POP {R7}
    BX LR

.end
