# Design Scope

- This project will only target 64-bit hosts; therefore, internal pointer sizes will always be 64-bit.

# Native data types

| data type | description                                                  | byte size |
| --------: | ------------------------------------------------------------ | --------: |
|      `i8` | signed 8-bit integer, two's complement                       |         1 |
|      `u8` | unsigned 8-bit integer                                       |         1 |
|    `bool` | boolean                                                      |         1 |
|     `i16` | signed 16-bit integer, two's complement, big-endian          |         2 |
|     `u16` | unsigned 16-bit integer, big-endian                          |         2 |
|     `i32` | signed 32-bit integer, two's complement, big-endian          |         4 |
|     `u32` | unsigned 32-bit integer, big-endian                          |         4 |
|     `f32` | 32-bit IEEE-754 floating-point number                        |         4 |
|     `i64` | signed 64-bit integer, two's complement, big-endian          |         8 |
|     `u64` | unsigned 64-bit integer, big-endian                          |         8 |
|     `f64` | 64-bit IEEE-754 floating-point number (double-precision)     |         8 |
|    `i128` | signed 128-bit integer, two's complement, big-endian         |        16 |
|    `u128` | unsigned 128-bit integer, big-endian                         |        16 |
|    `f128` | 128-bit IEEE-754 floating-point number (quadruple-precision) |        16 |




<!-- |     `ref` | [Reference-Counted Pointer](#reference-counted-pointers)     |        16 | -->

<!-- # Reference-counted Pointers

A `ref` or Reference-Counted Pointer is a pointer to a memory location combined with a `u64` counter for the number of times the pointer is referenced. This counter is incremented each time the pointer enters a new scope or is stored in a heap variable, and decrement when it leaves the scope or heap variable.

After it is decremented, if the counter has reached zero, the pointer is invalidated. Any attempt to access a `ref` with a counter of zero will result in undefined behavior and should not be achievable.

### Structure
\| data type |   field | start offset | length | end offset |
\| --------: | ------: | -----------: | -----: | ---------: |
\|     `ptr` | pointer |            0 |      8 |          8 |
\|     `u64` | counter |            8 |      8 |         16 |
\|           |         |    **TOTAL** | **16** |            | --> 

# VM

The VM is responsible for executing a program. The VM consists of many state data:

- The [stack](#stack)
- The [comparison register](#comparison-register)
- The [error register](#error-register)
- The [heap](#heap)
- The [call stack and instruction pointer](#program-execution)

# Stack

The stack is a 1 MB region of memory which is used while a program is executing.

Function arguments are pushed into the stack in FILO order (First-In-Last-Out) prior
to calling a function. A function SHOULD clean up the stack before it returns. A function
MUST push its return value to the stack such that it is in the same location on the stack
as was the start of the function parameters.

# Heap

TODO

# Program Execution

TODO

# Instruction Set

- Replace `<t>` with the set of all data types above (`i8`, `u8` ... `f128` etc.)
- Replace `<n>` with the set of all numeric data types above
- Replace `<i>` with the set of all SIGNED numeric data types above
- Replace `<s>` with the set of common data lengths (`{ 1, 2, 4, 8, 16, 32, 64 }`)

| instruction | parameters   | description                                                                            | notes                                                                                                                                           |
| ----------- | ------------ | -------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `push_<t>`  | `value:<t>`  | Push a literal `<t>` onto stack                                                        | -                                                                                                                                               |
| `loadl_<t>` | `index:u8`   | Pop `<t>` from stack and store in local #`index`                                       | -                                                                                                                                               |
| `pushl_<t>` | `index:u8`   | Push `<t>` from local #`index` to stack                                                | -                                                                                                                                               |
| `add_<n>`   | -            | Adds second stack value to top, overwriting top                                        | -                                                                                                                                               |
| `sub_<n>`   | -            | Subtracts second stack value from top, overwriting top                                 | -                                                                                                                                               |
| `mul_<n>`   | -            | Computes product of second stack value and top, overwriting top                        | -                                                                                                                                               |
| `div_<n>`   | -            | Computes quotient of second stack over the top value, overwriting the top              | If `<n>` is an integer type, this could be a lossy operation as floor division will be used. If the denominator is zero, behavior is undefined. |
| `rem_<n>`   | -            | Calculates the remainder of second stack value over the top value, overwriting the top | -                                                                                                                                               |
| `abs_<i>`   | -            | Computes absolute value of top of stack, pushing result                                | -                                                                                                                                               |
| `dupg`      | `n:u8`       | Duplicates and pushes `n` bytes from top of stack                                      | -                                                                                                                                               |
| `dup<s>`    | -            | Duplicates and pushes `s` from top of stack                                            | -                                                                                                                                               |
| `cmp_<n>`   | -            | Compares top two values in stack, storing result in comparison register                | See [Comparison Register](#comparison-register) for return information.                                                                         |
| `jz`        | `addr:u64`   | Jumps if comparison register holds zero                                                | -                                                                                                                                               |
| `jnz`       | `addr:u64`   | Jumps if comparison register holds non-zero                                            | -                                                                                                                                               |
| `jl`        | `addr:u64`   | Jumps if comparison register holds `-1`                                                | -                                                                                                                                               |
| `jg`        | `addr:u64`   | Jumps if comparison register holds `1`                                                 | -                                                                                                                                               |
| `jmp`       | `addr:u64`   | Unconditionally jumps                                                                  | -                                                                                                                                               |
| `swapg`     | `n:u8`       | Swaps `n` bytes on the top of stack                                                    | -                                                                                                                                               |
| `swap<s>`   | -            | Swaps `s` bytes on the top of stack                                                    | -                                                                                                                                               |
| `popg`      | `n:u8`       | Pops `n` bytes from the stack                                                          | -                                                                                                                                               |
| `pop<s>`    | -            | Pops `s` bytes from the stack                                                          | -                                                                                                                                               |
| `ret`       | -            | Returns from the current subroutine                                                    | -                                                                                                                                               |
| `dbg`       | `i:u64`      | Triggers a debugger breakpoint with the specified ID.                                  | -                                                                                                                                               |
| `sig`       | `signal:i64` | Triggers a crash with the specified code.                                              | -                                                                                                                                               |

# Comparison Register

Based on the result of a numeric comparison operation (`cmp_<n>`), one of the following `i8` values will be stored in the comparison register,
where `A` is the second-to-top stack value and `B` is the top stack value:

| condition | value |
| --------- | ----: |
| `A < B`   |    -1 |
| `A == B`  |     0 |
| `A > B`   |     1 |

# Error Register

TODO - will be set upon undefined math operations such as division by zero.
