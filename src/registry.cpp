#include "registry.h"

namespace pushle {

TokenRegistry *TokenRegistry::instance = nullptr;

TokenRegistry &TokenRegistry::getInstance() {
  if (instance != nullptr) {
    return *instance;
  }
  instance = new TokenRegistry();

  #pragma region push
  instance->registerToken(Op::PUSH_I8,    "push_i8",    {DataType::_i8});
  instance->registerToken(Op::PUSH_U8,    "push_u8",    {DataType::_u8});
  instance->registerToken(Op::PUSH_BOOL,  "push_bool",  {DataType::_bool});
  instance->registerToken(Op::PUSH_I16,   "push_i16",   {DataType::_i16});
  instance->registerToken(Op::PUSH_U16,   "push_u16",   {DataType::_u16});
  instance->registerToken(Op::PUSH_I32,   "push_i32",   {DataType::_i32});
  instance->registerToken(Op::PUSH_U32,   "push_u32",   {DataType::_u32});
  instance->registerToken(Op::PUSH_F32,   "push_f32",   {DataType::_f32});
  instance->registerToken(Op::PUSH_I64,   "push_i64",   {DataType::_i64});
  instance->registerToken(Op::PUSH_U64,   "push_u64",   {DataType::_u64});
  instance->registerToken(Op::PUSH_F64,   "push_f64",   {DataType::_f64});
  #pragma endregion push

  #pragma region pushl
  instance->registerToken(Op::PUSHL_I8,   "pushl_i8",   {DataType::_u8});
  instance->registerToken(Op::PUSHL_U8,   "pushl_u8",   {DataType::_u8});
  instance->registerToken(Op::PUSHL_BOOL, "pushl_bool", {DataType::_u8});
  instance->registerToken(Op::PUSHL_I16,  "pushl_i16",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_U16,  "pushl_u16",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_I32,  "pushl_i32",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_U32,  "pushl_u32",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_F32,  "pushl_f32",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_I64,  "pushl_i64",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_U64,  "pushl_u64",  {DataType::_u8});
  instance->registerToken(Op::PUSHL_F64,  "pushl_f64",  {DataType::_u8});
  #pragma endregion pushl

  #pragma region popl
  instance->registerToken(Op::POPL_I8,    "popl_i8",    {DataType::_u8});
  instance->registerToken(Op::POPL_U8,    "popl_u8",    {DataType::_u8});
  instance->registerToken(Op::POPL_BOOL,  "popl_bool",  {DataType::_u8});
  instance->registerToken(Op::POPL_I16,   "popl_i16",   {DataType::_u8});
  instance->registerToken(Op::POPL_U16,   "popl_u16",   {DataType::_u8});
  instance->registerToken(Op::POPL_I32,   "popl_i32",   {DataType::_u8});
  instance->registerToken(Op::POPL_U32,   "popl_u32",   {DataType::_u8});
  instance->registerToken(Op::POPL_F32,   "popl_f32",   {DataType::_u8});
  instance->registerToken(Op::POPL_I64,   "popl_i64",   {DataType::_u8});
  instance->registerToken(Op::POPL_U64,   "popl_u64",   {DataType::_u8});
  instance->registerToken(Op::POPL_F64,   "popl_f64",   {DataType::_u8});
  #pragma endregion popl

  #pragma region setl
  instance->registerToken(Op::SETL_I8,    "setl_i8",    {DataType::_u8,DataType::_i8});
  instance->registerToken(Op::SETL_U8,    "setl_u8",    {DataType::_u8,DataType::_u8});
  instance->registerToken(Op::SETL_BOOL,  "setl_bool",  {DataType::_u8,DataType::_bool});
  instance->registerToken(Op::SETL_I16,   "setl_i16",   {DataType::_u8,DataType::_i16});
  instance->registerToken(Op::SETL_U16,   "setl_u16",   {DataType::_u8,DataType::_u16});
  instance->registerToken(Op::SETL_I32,   "setl_i32",   {DataType::_u8,DataType::_i32});
  instance->registerToken(Op::SETL_U32,   "setl_u32",   {DataType::_u8,DataType::_u32});
  instance->registerToken(Op::SETL_F32,   "setl_f32",   {DataType::_u8,DataType::_f32});
  instance->registerToken(Op::SETL_I64,   "setl_i64",   {DataType::_u8,DataType::_i64});
  instance->registerToken(Op::SETL_U64,   "setl_u64",   {DataType::_u8,DataType::_u64});
  instance->registerToken(Op::SETL_F64,   "setl_f64",   {DataType::_u8,DataType::_f64});
  #pragma endregion setl

  #pragma region add
  instance->registerToken(Op::ADD_I8,     "add_i8",     {});
  instance->registerToken(Op::ADD_U8,     "add_u8",     {});
  instance->registerToken(Op::ADD_I16,    "add_i16",    {});
  instance->registerToken(Op::ADD_U16,    "add_u16",    {});
  instance->registerToken(Op::ADD_I32,    "add_i32",    {});
  instance->registerToken(Op::ADD_U32,    "add_u32",    {});
  instance->registerToken(Op::ADD_F32,    "add_f32",    {});
  instance->registerToken(Op::ADD_I64,    "add_i64",    {});
  instance->registerToken(Op::ADD_U64,    "add_u64",    {});
  instance->registerToken(Op::ADD_F64,    "add_f64",    {});
  #pragma endregion add

  #pragma region sub
  instance->registerToken(Op::SUB_I8,     "sub_i8",     {});
  instance->registerToken(Op::SUB_U8,     "sub_u8",     {});
  instance->registerToken(Op::SUB_I16,    "sub_i16",    {});
  instance->registerToken(Op::SUB_U16,    "sub_u16",    {});
  instance->registerToken(Op::SUB_I32,    "sub_i32",    {});
  instance->registerToken(Op::SUB_U32,    "sub_u32",    {});
  instance->registerToken(Op::SUB_F32,    "sub_f32",    {});
  instance->registerToken(Op::SUB_I64,    "sub_i64",    {});
  instance->registerToken(Op::SUB_U64,    "sub_u64",    {});
  instance->registerToken(Op::SUB_F64,    "sub_f64",    {});
  #pragma endregion sub

  #pragma region mul
  instance->registerToken(Op::MUL_I8,     "mul_i8",     {});
  instance->registerToken(Op::MUL_U8,     "mul_u8",     {});
  instance->registerToken(Op::MUL_I16,    "mul_i16",    {});
  instance->registerToken(Op::MUL_U16,    "mul_u16",    {});
  instance->registerToken(Op::MUL_I32,    "mul_i32",    {});
  instance->registerToken(Op::MUL_U32,    "mul_u32",    {});
  instance->registerToken(Op::MUL_F32,    "mul_f32",    {});
  instance->registerToken(Op::MUL_I64,    "mul_i64",    {});
  instance->registerToken(Op::MUL_U64,    "mul_u64",    {});
  instance->registerToken(Op::MUL_F64,    "mul_f64",    {});
  #pragma endregion mul

  #pragma region div
  instance->registerToken(Op::DIV_I8,     "div_i8",     {});
  instance->registerToken(Op::DIV_U8,     "div_u8",     {});
  instance->registerToken(Op::DIV_I16,    "div_i16",    {});
  instance->registerToken(Op::DIV_U16,    "div_u16",    {});
  instance->registerToken(Op::DIV_I32,    "div_i32",    {});
  instance->registerToken(Op::DIV_U32,    "div_u32",    {});
  instance->registerToken(Op::DIV_F32,    "div_f32",    {});
  instance->registerToken(Op::DIV_I64,    "div_i64",    {});
  instance->registerToken(Op::DIV_U64,    "div_u64",    {});
  instance->registerToken(Op::DIV_F64,    "div_f64",    {});
  #pragma endregion div

  #pragma region rem
  instance->registerToken(Op::REM_I8,     "rem_i8",     {});
  instance->registerToken(Op::REM_U8,     "rem_u8",     {});
  instance->registerToken(Op::REM_I16,    "rem_i16",    {});
  instance->registerToken(Op::REM_U16,    "rem_u16",    {});
  instance->registerToken(Op::REM_I32,    "rem_i32",    {});
  instance->registerToken(Op::REM_U32,    "rem_u32",    {});
  instance->registerToken(Op::REM_F32,    "rem_f32",    {});
  instance->registerToken(Op::REM_I64,    "rem_i64",    {});
  instance->registerToken(Op::REM_U64,    "rem_u64",    {});
  instance->registerToken(Op::REM_F64,    "rem_f64",    {});
  #pragma endregion rem

  #pragma region abs
  instance->registerToken(Op::ABS_I8,     "abs_i8",     {});
  instance->registerToken(Op::ABS_I16,    "abs_i16",    {});
  instance->registerToken(Op::ABS_I32,    "abs_i32",    {});
  instance->registerToken(Op::ABS_I64,    "abs_i64",    {});
  instance->registerToken(Op::ABS_F32,    "abs_f32",    {});
  instance->registerToken(Op::ABS_F64,    "abs_f64",    {});
  #pragma endregion abs

  #pragma region inc
  instance->registerToken(Op::INC_I8,     "inc_i8",     {});
  instance->registerToken(Op::INC_U8,     "inc_u8",     {});
  instance->registerToken(Op::INC_I16,    "inc_i16",    {});
  instance->registerToken(Op::INC_U16,    "inc_u16",    {});
  instance->registerToken(Op::INC_I32,    "inc_i32",    {});
  instance->registerToken(Op::INC_U32,    "inc_u32",    {});
  instance->registerToken(Op::INC_F32,    "inc_f32",    {});
  instance->registerToken(Op::INC_I64,    "inc_i64",    {});
  instance->registerToken(Op::INC_U64,    "inc_u64",    {});
  instance->registerToken(Op::INC_F64,    "inc_f64",    {});
  #pragma endregion inc

  #pragma region dec
  instance->registerToken(Op::DEC_I8,     "dec_i8",     {});
  instance->registerToken(Op::DEC_U8,     "dec_u8",     {});
  instance->registerToken(Op::DEC_I16,    "dec_i16",    {});
  instance->registerToken(Op::DEC_U16,    "dec_u16",    {});
  instance->registerToken(Op::DEC_I32,    "dec_i32",    {});
  instance->registerToken(Op::DEC_U32,    "dec_u32",    {});
  instance->registerToken(Op::DEC_F32,    "dec_f32",    {});
  instance->registerToken(Op::DEC_I64,    "dec_i64",    {});
  instance->registerToken(Op::DEC_U64,    "dec_u64",    {});
  instance->registerToken(Op::DEC_F64,    "dec_f64",    {});
  #pragma endregion dec

  #pragma region dup
  instance->registerToken(Op::DUPG,       "dupg",       {DataType::_u8});
  instance->registerToken(Op::DUP1,       "dup1",       {});
  instance->registerToken(Op::DUP2,       "dup2",       {});
  instance->registerToken(Op::DUP4,       "dup4",       {});
  instance->registerToken(Op::DUP8,       "dup8",       {});
  instance->registerToken(Op::DUP16,      "dup16",      {});
  #pragma endregion dup

  #pragma region swap
  instance->registerToken(Op::SWAPG,      "swapg",      {DataType::_u8});
  instance->registerToken(Op::SWAP1,      "swap1",      {});
  instance->registerToken(Op::SWAP2,      "swap2",      {});
  instance->registerToken(Op::SWAP4,      "swap4",      {});
  instance->registerToken(Op::SWAP8,      "swap8",      {});
  instance->registerToken(Op::SWAP16,     "swap16",     {});
  #pragma endregion swap

  #pragma region pop
  instance->registerToken(Op::POPG,       "popg",       {DataType::_u8});
  instance->registerToken(Op::POP1,       "pop1",       {});
  instance->registerToken(Op::POP2,       "pop2",       {});
  instance->registerToken(Op::POP4,       "pop4",       {});
  instance->registerToken(Op::POP8,       "pop8",       {});
  instance->registerToken(Op::POP16,      "pop16",      {});
  #pragma endregion pop

  #pragma region cmp
  instance->registerToken(Op::CMP_I8,     "cmp_i8",     {});
  instance->registerToken(Op::CMP_U8,     "cmp_u8",     {});
  instance->registerToken(Op::CMP_I16,    "cmp_i16",    {});
  instance->registerToken(Op::CMP_U16,    "cmp_u16",    {});
  instance->registerToken(Op::CMP_I32,    "cmp_i32",    {});
  instance->registerToken(Op::CMP_U32,    "cmp_u32",    {});
  instance->registerToken(Op::CMP_F32,    "cmp_f32",    {});
  instance->registerToken(Op::CMP_I64,    "cmp_i64",    {});
  instance->registerToken(Op::CMP_U64,    "cmp_u64",    {});
  instance->registerToken(Op::CMP_F64,    "cmp_f64",    {});
  #pragma endregion cmp

  #pragma region jmp
  instance->registerToken(Op::JZ,         "jz",         {DataType::_u64});
  instance->registerToken(Op::JNZ,        "jnz",        {DataType::_u64});
  instance->registerToken(Op::JL,         "jl",         {DataType::_u64});
  instance->registerToken(Op::JG,         "jg",         {DataType::_u64});
  instance->registerToken(Op::JNL,        "jnl",        {DataType::_u64});
  instance->registerToken(Op::JNG,        "jng",        {DataType::_u64});
  instance->registerToken(Op::JMP,        "jmp",        {DataType::_u64});
  #pragma endregion jmp

  #pragma region exec
  instance->registerToken(Op::RET,        "ret",        {});
  instance->registerToken(Op::DBG,        "dbg",        {DataType::_i8});
  instance->registerToken(Op::SIG,        "sig",        {DataType::_i8});
  #pragma endregion exec

  return *instance;
}

}
