(main)
@4
D=A
@13
M=D
(main_SET_LCL)
@SP
A=M
M=0
@SP
M=M+1
@13
M=M-1
D=M
@main_SET_LCL
null=D;JNE
(STARTING)
@10
D=A
@SP
A=M
M=D
@SP
M=M+1
@6
D=A
@5
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@0
D=A
@LCL
A=D+A
D=M
@SP
M=D
M=M+1
@5
D=A
@THAT
A=D+A
D=M
@SP
M=D
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
M=M+D
@SP
M=M+1
@1
D=A
@ARG
A=D+A
D=M
@SP
M=D
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
M=M-D
@SP
M=M+1
@SP
AM=M-1
M=-M
@SP
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
D=D&M
@TRUE_CONDITION_AND_0
D;JNE
@SP
A=M
M=0
(TRUE_CONDITION_AND_0)
@SP
A=M
M=1
@SP
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
D=M-D
@TRUE_CONDITION_GT_0
D;JGT
@SP
A=M
M=0
(TRUE_CONDITION_GT_0)
@SP
A=M
M=1
@SP
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
D=M-D
@TRUE_CONDITION_LT_0
D;JLT
@SP
A=M
M=0
(TRUE_CONDITION_LT_0)
@SP
A=M
M=1
@SP
M=M+1
@SP
AM=M-1
D=M
@13
M=D
@SP
AM=M-1
D=M
@13
D=M-D
@TRUE_CONDITION_LT_1
D;JLT
@SP
A=M
M=0
(TRUE_CONDITION_LT_1)
@SP
A=M
M=1
@SP
M=M+1
@test.5
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@test.5
M=D
@STARTING
null=null;JMP
@main$ret.0
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=A
@5
D=A+D
@SP
D=M-D
@ARG
M=D
@SP
D=M
@LCL
M=D
@foo
null=null;JMP
(main$ret.0)
@main$ret.1
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=A
@5
D=A+D
@SP
D=M-D
@ARG
M=D
@SP
D=M
@LCL
M=D
@foo
null=null;JMP
(main$ret.1)
@LCL
D=M
@13
M=D
@5
D=D-A
@14
M=D
@SP
AM=M-1
D=M
@ARG
M=D
D=M
@SP
M=D+1
@13
D=M
@THAT
MD=D-1
@THIS
MD=D-1
@ARG
MD=D-1
@LCL
MD=D-1
@14
A=M
null=null;JMP
(foo)
@3
D=A
@13
M=D
(foo_SET_LCL)
@SP
A=M
M=0
@SP
M=M+1
@13
M=M-1
D=M
@foo_SET_LCL
null=D;JNE
@5
D=A
@LCL
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@1
D=A
@ARG
A=D+A
D=M
@SP
M=D
M=M+1
@LCL
D=M
@13
M=D
@5
D=D-A
@14
M=D
@SP
AM=M-1
D=M
@ARG
M=D
D=M
@SP
M=D+1
@13
D=M
@THAT
MD=D-1
@THIS
MD=D-1
@ARG
MD=D-1
@LCL
MD=D-1
@14
A=M
null=null;JMP
