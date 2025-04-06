_start:
    mov R2, 0

    mov R4, 0

    mov R3, 1

multiply_loop:
    and R4, R1, R3
    
    cmp R4, 0
    je skip_addition

    add R2, R0

skip_addition:
    shl R3, 1

    shl R0, 1

    cmp R3, 0            
    jne multiply_loop    

    ret