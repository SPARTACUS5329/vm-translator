**Objective:** To generate assembly code from VM code
**Example VM code**
```BasicTest.vm
push constant 10
pop temp 6
push local 0
push that 5
add
push argument 1
sub
neg
```

## Implementing Virtual Memory Segments
1. `local`
2. `argument`
3. `this`
4. `that`
5. `constant`
6. `static`
7. `pointer`
8. `temp`

### Implementing the stack
**Assumptions**
- `Stack Pointer (SP)` - Stored in `RAM[0]`
- Stack base `addr = 256`

**Example:**
Here `i` is any number
```VM
push constant i
```

```Pseudo-Assembly
*SP=i
SP++
```

```Hack-Assembly
@17 // D = i
D=A

@SP // *SP = D
A=M
M=D

@SP // SP++
M=M+1
```

### Implementing `local`

**Assumptions**
- `Local Pointer (LCL)` - Stored in `RAM[1]`
- Local base `addr = 1015`

**Example:**
Here `i` is any number
```VM
pop local i
```

```Pseudo-Assembly
SP--
addr=LCL+i
*addr=*SP
```

```Hack-Assembly
@i // D=i
D=A
@LCL // D=LCL+D
D=D+A
@R13
M=D

@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@R13 // A=LCL+i, M=Stack[-1]
A=M
M=D
```

**Example:**
Here `i` is any number
```VM
push local i
```

```Pseudo-Assembly
addr=LCL+i
*SP=*addr
SP++
```

```Hack-Assembly
@i // addr=LCL+i
D=A
@LCL
A=D+A

D=M // D=*addr

@SP // *SP=*addr, SP++
M=D
M=M+1
```

These 2 examples can be extended to the general cases ^3bc85f
1. `push segment i`
2. `pop segment i`

Therefore this can be extended to `argument`, `this`, and `that`

**Base addresses:**
1. `ARG - RAM[2]`
2. `THIS - RAM[3]`
3. `THAT - RAM[4]`

### Implementing `static`

**Challenge:** The `static` variables should be seen by all the methods in a program
**Solution:** Store them in a `global space`

Reference all the `command static i` in `Foo.vm` as `Foo.i` where `i` is any number
The HACK assembler will map these variables into `RAM[16] - RAM[255]`. Therefore these are accessible by all methods in a program.

### Implementing `temp`

It is a fixed `8` place memory segment with base address as `RAM[5]`. Therefore it extends till `RAM[12]`. The `push` and `pop` commands can be implemented like the [[VM Translator#^3bc85f|general cases]] with the base address hardcoded as `5`.

### Implementing `pointer`

Here we store the base address of `this` and `that`. So, this only occupies two registers. The only valid operations are
- `push pointer 0/1`
- `pop pointer 0/1`

References
- `0` => `this`
- `1` => `that`

**Example:**
```VM
push pointer 0/1
```

```Pseudo-Assembly
*SP=THIS/THAT
SP++
```

```Hack-Assembly
@THIS/THAT
D=A

@SP
M=D
M=M+1
```

**Example:**
```VM
pop pointer 0/1
```

```Pseudo-Assembly
SP--
THIS/THAT=*SP
```

```Hack-Assembly
@SP
AM=M-1
D=M

@THIS/THAT
M=D
```

## Implementing Arithmetic and Logical Operations

#### 1. `ADD`
**Action:** `Pop`s the last two elements in the stack, adds them and `push`es the result

```VM
add
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr=*SP + *13
*SP=*addr
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] + Stack[-2]
M=M+D

@SP // SP++
M=M+1
```

#### 2. `SUB`
**Action:** `Pop`s the last two elements in the stack, subtracts the first element from the second and `push`es the result

```VM
sub
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr= *SP - *13
*SP=*addr
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] - Stack[-2]
M=M-D

@SP // SP++
M=M+1
```

#### 3. `NEG`
**Action:** `Pop`s the last element in the stack, negates it and `push`es it

```VM
neg
```

```Pseudo-Assembly
SP--
*SP=-*SP
SP++
```

```Hack-Assembly
@SP // SP--
AM=M-1

M=-M // *SP = -*SP

@SP // SP++
M=M+1
```

#### 4. `EQ`
**Action:** `Pop`s the last two elements in the stack, checks if their equal and `push`es the boolean result

```VM
eq
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr= *SP - *13; JEQ TO TRUE_CONDITION
*SP=0

TRUE_CONDITION:
*SP=1
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] - Stack[-2]
D=M-D

@TRUE_CONDITION_EQ_i
D;JEQ

@SP
A=M
M=0

(TRUE_CONDITION_EQ_i)
@SP
A=M
M=1

@SP
M=M+1
```

Here `i` is the number of times `EQ` has already been called. So the first instance of `EQ` will have a label `TRUE_CONDITION_EQ_0`

#### 5. `GT`
**Action:** `Pop`s the last two elements in the stack, checks if the second is greater than the first and `push`es the boolean result

```VM
gt
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr= *SP - *13; JGT TO TRUE_CONDITION
*SP=0

TRUE_CONDITION:
*SP=1
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] - Stack[-2]
D=M-D

@TRUE_CONDITION_GT_i
D;JGT

@SP
A=M
M=0

(TRUE_CONDITION_GT_i)
@SP
A=M
M=1

@SP
M=M+1
```

Here `i` is the number of times `GT` has already been called. So the first instance of `EQ` will have a label `TRUE_CONDITION_GT_0`

#### 6. `LT`
**Action:** `Pop`s the last two elements in the stack, checks if the second one is less than the first one and `push`es the boolean result

```VM
lt
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr= *SP - *13; JLT TO TRUE_CONDITION
*SP=0

TRUE_CONDITION:
*SP=1
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] - Stack[-2]
D=M-D

@TRUE_CONDITION_LT_i
D;JLT

@SP
A=M
M=0

(TRUE_CONDITION_LT_i)
@SP
A=M
M=1

@SP
M=M+1
```

Here `i` is the number of times `LT` has already been called. So the first instance of `EQ` will have a label `TRUE_CONDITION_LT_0`

#### 7. `AND`
**Action:** `Pop`s the last two elements in the stack, *boolean ANDs* them and `push`es the boolean result

```VM
and
```

```Pseudo-Assembly
SP--
addr=13
*addr=*SP
SP--
addr=13
*addr= *SP && *13; JNE TO TRUE_CONDITION
*SP=0

TRUE_CONDITION:
*SP=1
SP++
```

```Hack-Assembly
@SP // SP--, D=Stack[-1]
AM=M-1
D=M

@13
M=D

@SP // SP--, D=Stack[-2]
AM=M-1
D=M

@13 // *13 = Stack[-1] - Stack[-2]
D=D&M

@TRUE_CONDITION_AND_i
D;JNE

@SP
A=M
M=0

(TRUE_CONDITION_AND_i)
@SP
A=M
M=1

@SP
M=M+1
```

Here `i` is the number of times `AND` has already been called. So the first instance of `EQ` will have a label `TRUE_CONDITION_AND_0`
#### 8. `NOT`
**Action:** `Pop`s the last element, *boolean NOTs* it and `push`es the boolean result

```VM
not
```

```Pseudo-Assembly
SP--
*SP=!*SP
SP++
```

```Hack-Assembly
@SP // SP--
AM=M-1

M=!M // *SP = !*SP

@SP // SP++
M=M+1
```



