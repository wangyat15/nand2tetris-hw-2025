@5    // Load the value 5
D=A   // Store it in D register
@R0   // Store 5 in R0
M=D   

@0    // Load the value 0
D=A   // Store it in D register
@R1   // Store 0 in R1
M=D   

@R0   // Load value from R0
D=M   // Store R0 value in D

@R1   // Add value from R1
D=D+M // Add the value of R1 to D

@16   // Store the result in RAM[16]
M=D   
