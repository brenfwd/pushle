#pragma once

#define _OP_T(prefix) \
  prefix##I8, \
  prefix##U8, \
  prefix##BOOL, \
  prefix##I16, \
  prefix##U16, \
  prefix##I32, \
  prefix##U32, \
  prefix##F32, \
  prefix##I64, \
  prefix##U64, \
  prefix##F64, \
  prefix##I128, \
  prefix##U128, \
  prefix##F128

#define _OP_N(prefix) \
  prefix##I8, \
  prefix##U8, \
  prefix##I16, \
  prefix##U16, \
  prefix##I32, \
  prefix##U32, \
  prefix##F32, \
  prefix##I64, \
  prefix##U64, \
  prefix##F64, \
  prefix##I128, \
  prefix##U128, \
  prefix##F128

#define _OP_I(prefix) \
  prefix##I8, \
  prefix##I16, \
  prefix##I32, \
  prefix##F32, \
  prefix##I64, \
  prefix##F64, \
  prefix##I128, \
  prefix##F128

#define _OP_S(prefix) \
  prefix##1, \
  prefix##2, \
  prefix##4, \
  prefix##8, \
  prefix##16

namespace lang {
  enum Op {
    _OP_T(PUSH_),
    _OP_T(POPL_),
    _OP_T(PUSHL_),
    _OP_T(SETL_),
    
    _OP_N(ADD_),
    _OP_N(SUB_), 
    _OP_N(MUL_),
    _OP_N(DIV_),
    _OP_N(REM_),
    _OP_I(ABS_),

    _OP_N(DEC_),
    _OP_N(INC_),

    DUPG,
    _OP_S(DUP),

    SWAPG,
    _OP_S(SWAP),
    POPG,
    _OP_S(POP),

    _OP_N(CMP_),
    JZ,
    JNZ,
    JL,
    JG,
    JNL,
    JNG,
    JMP,

    RET,
    DBG,
    SIG,
  };
};
