@KBD
D=M
@INFINITE_LOOP
D;JNE

@SCREEN
D=A
@addr
M=D

@8192
D=A
@paddle
M=D

(LOOP)
@KBD
D=M
@LEFT
D;JEQ
@KBD
D=M
@RIGHT
D;JGT
@LOOP
0;JMP

(LEFT)
@paddle
M=M-1
@DRAW
0;JMP

(RIGHT)
@paddle
M=M+1

(DRAW)
@paddle
D=M
@addr
M=D
@0
D=A
@i
M=D

(DRAW_LOOP)
@i
D=M
@16
D=D-A
@LOOP
D;JGE

@addr
A=M
M=-1

@addr
M=M+1
@i
M=M+1
@DRAW_LOOP
0;JMP

(INFINITE_LOOP)
@INFINITE_LOOP
0;JMP