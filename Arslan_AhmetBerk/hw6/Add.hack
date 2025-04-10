@R0
M=5  // Set R0 to 5

@R1
M=0  // Set R1 to 0

@R0
D=M  // Load the value of R0 into D

@R1
D=D+M  // Add the value of R1 to D

@16
M=D  // Store the result in RAM[16]
