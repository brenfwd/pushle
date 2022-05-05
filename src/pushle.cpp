#include "pushle.h"

#include <exception>
#include <stdexcept>

namespace pushle {

VM::VM() {
  for (size_t i = 0; i < VM_STACK_SIZE; i++)
    stack[i] = 0;
  stack_top = nullptr;

  program = nullptr;
  program_size = 0;
  instruction = nullptr;

  reg_cmp = 0;
  reg_err = 0;
  reg_ret = nullptr;

  // TODO: heap

  VM_DEBUG_1("VM initialized");
}

void VM::run(const uint8_t *program, size_t size) {
  this->program = program;
  this->program_size = size;
  instruction = program;
  // TODO: locate start instruction and set instruction pointer
  while (step()) {
    // usleep(10000);
    VM_DEBUG_2("");
    VM_DEBUG_2("");
  }
}

void *VM::read(size_t size) {
  VM_DEBUG_2("->read {}", size);
  if (instruction + size > program + program_size) {
    throw std::runtime_error("read(): out of bounds");
  }
  this->instruction += size;
  return (void *)(this->instruction - size);
}

bool VM::step() { // returns false if VM is finished
  VM_DEBUG_2("(step)");
  if (instruction == nullptr || instruction >= program + program_size) {
    VM_DEBUG_1("(done)");
    return false;
  }

  Op opcode = (Op) *instruction;
  instruction++;

  switch (opcode) {
    case PUSH_I8:   VM_DEBUG_2("i:PUSH_I8");         push_i8(*(int8_t *)read(1)); break;
    case PUSH_U8:   VM_DEBUG_2("i:PUSH_U8");         push_u8(*(uint8_t *)read(1)); break;
    case PUSH_BOOL: VM_DEBUG_2("i:PUSH_BOOL");       push_bool(*(bool *)read(1)); break;
    case PUSH_I16:  VM_DEBUG_2("i:PUSH_I16");        push_i16(*(int16_t *)read(2)); break;
    case PUSH_U16:  VM_DEBUG_2("i:PUSH_U16");        push_u16(*(uint16_t *)read(2)); break;
    case PUSH_I32:  VM_DEBUG_2("i:PUSH_I32");        push_i32(*(int32_t *)read(4)); break;
    case PUSH_U32:  VM_DEBUG_2("i:PUSH_U32");        push_u32(*(uint32_t *)read(4)); break;
    case PUSH_F32:  VM_DEBUG_2("i:PUSH_F32");        push_f32(*(float *)read(4)); break;
    case PUSH_I64:  VM_DEBUG_2("i:PUSH_I64");        push_i64(*(int64_t *)read(8)); break;
    case PUSH_U64:  VM_DEBUG_2("i:PUSH_U64");        push_u64(*(uint64_t *)read(8)); break;
    case PUSH_F64:  VM_DEBUG_2("i:PUSH_F64");        push_f64(*(double *)read(8)); break;
    
    case POPL_I8:  VM_DEBUG_2("i:POPL_I8");          popl_i8(&scope, *(uint8_t *)read(1)); break;
    case POPL_U8:  VM_DEBUG_2("i:POPL_U8");          popl_u8(&scope, *(uint8_t *)read(1)); break;
    case POPL_BOOL:VM_DEBUG_2("i:POPL_BOOL");        popl_bool(&scope, *(uint8_t *)read(1)); break;
    case POPL_I16: VM_DEBUG_2("i:POPL_I16");         popl_i16(&scope, *(uint8_t *)read(1)); break;
    case POPL_U16: VM_DEBUG_2("i:POPL_U16");         popl_u16(&scope, *(uint8_t *)read(1)); break;
    case POPL_I32: VM_DEBUG_2("i:POPL_I32");         popl_i32(&scope, *(uint8_t *)read(1)); break;
    case POPL_U32: VM_DEBUG_2("i:POPL_U32");         popl_u32(&scope, *(uint8_t *)read(1)); break;
    case POPL_F32: VM_DEBUG_2("i:POPL_F32");         popl_f32(&scope, *(uint8_t *)read(1)); break;
    case POPL_I64: VM_DEBUG_2("i:POPL_I64");         popl_i64(&scope, *(uint8_t *)read(1)); break;
    case POPL_U64: VM_DEBUG_2("i:POPL_U64");         popl_u64(&scope, *(uint8_t *)read(1)); break;
    case POPL_F64: VM_DEBUG_2("i:POPL_F64");         popl_f64(&scope, *(uint8_t *)read(1)); break;
      
    case PUSHL_I8:  VM_DEBUG_2("i:PUSHL_I8");        pushl_i8(&scope, *(int8_t *)read(1)); break;
    case PUSHL_U8:  VM_DEBUG_2("i:PUSHL_U8");        pushl_u8(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_BOOL:VM_DEBUG_2("i:PUSHL_BOOL");      pushl_bool(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_I16: VM_DEBUG_2("i:PUSHL_I16");       pushl_i16(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_U16: VM_DEBUG_2("i:PUSHL_U16");       pushl_u16(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_I32: VM_DEBUG_2("i:PUSHL_I32");       pushl_i32(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_U32: VM_DEBUG_2("i:PUSHL_U32");       pushl_u32(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_F32: VM_DEBUG_2("i:PUSHL_F32");       pushl_f32(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_I64: VM_DEBUG_2("i:PUSHL_I64");       pushl_i64(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_U64: VM_DEBUG_2("i:PUSHL_U64");       pushl_u64(&scope, *(uint8_t *)read(1)); break;
    case PUSHL_F64: VM_DEBUG_2("i:PUSHL_F64");       pushl_f64(&scope, *(uint8_t *)read(1)); break;
    
    case SETL_I8:   VM_DEBUG_2("i:SETL_I8");        { int8_t index = *(uint8_t *)read(1);  int8_t      value = *(int8_t *)read(1);       setl_i8(value, &scope, index);   break; }
    case SETL_U8:   VM_DEBUG_2("i:SETL_U8");        { int8_t index = *(uint8_t *)read(1);  uint8_t     value = *(uint8_t *)read(1);      setl_u8(value, &scope, index);   break; }
    case SETL_BOOL: VM_DEBUG_2("i:SETL_BOOL");      { int8_t index = *(uint8_t *)read(1);  bool        value = *(bool *)read(1);         setl_bool(value, &scope, index); break; }
    case SETL_I16:  VM_DEBUG_2("i:SETL_I16");       { int8_t index = *(uint8_t *)read(1);  int16_t     value = *(int16_t *)read(2);      setl_i16(value, &scope, index);  break; }
    case SETL_U16:  VM_DEBUG_2("i:SETL_U16");       { int8_t index = *(uint8_t *)read(1);  uint16_t    value = *(uint16_t *)read(2);     setl_u16(value, &scope, index);  break; }
    case SETL_I32:  VM_DEBUG_2("i:SETL_I32");       { int8_t index = *(uint8_t *)read(1);  int32_t     value = *(int32_t *)read(4);      setl_i32(value, &scope, index);  break; }
    case SETL_U32:  VM_DEBUG_2("i:SETL_U32");       { int8_t index = *(uint8_t *)read(1);  uint32_t    value = *(uint32_t *)read(4);     setl_u32(value, &scope, index);  break; }
    case SETL_F32:  VM_DEBUG_2("i:SETL_F32");       { int8_t index = *(uint8_t *)read(1);  float       value = *(float *)read(4);        setl_f32(value, &scope, index);  break; }
    case SETL_I64:  VM_DEBUG_2("i:SETL_I64");       { int8_t index = *(uint8_t *)read(1);  int64_t     value = *(int64_t *)read(8);      setl_i64(value, &scope, index);  break; }
    case SETL_U64:  VM_DEBUG_2("i:SETL_U64");       { int8_t index = *(uint8_t *)read(1);  uint64_t    value = *(uint64_t *)read(8);     setl_u64(value, &scope, index);  break; }
    case SETL_F64:  VM_DEBUG_2("i:SETL_F64");       { int8_t index = *(uint8_t *)read(1);  double      value = *(double *)read(8);       setl_f64(value, &scope, index);  break; }

    case ADD_I8:    VM_DEBUG_2("i:ADD_I8");          add_i8(); break;
    case ADD_U8:    VM_DEBUG_2("i:ADD_U8");          add_u8(); break;
    case ADD_I16:   VM_DEBUG_2("i:ADD_I16");         add_i16(); break;
    case ADD_U16:   VM_DEBUG_2("i:ADD_U16");         add_u16(); break;
    case ADD_I32:   VM_DEBUG_2("i:ADD_I32");         add_i32(); break;
    case ADD_U32:   VM_DEBUG_2("i:ADD_U32");         add_u32(); break;
    case ADD_F32:   VM_DEBUG_2("i:ADD_F32");         add_f32(); break;
    case ADD_I64:   VM_DEBUG_2("i:ADD_I64");         add_i64(); break;
    case ADD_U64:   VM_DEBUG_2("i:ADD_U64");         add_u64(); break;
    case ADD_F64:   VM_DEBUG_2("i:ADD_F64");         add_f64(); break;
    
    case SUB_I8:    VM_DEBUG_2("i:SUB_I8");          sub_i8(); break;
    case SUB_U8:    VM_DEBUG_2("i:SUB_U8");          sub_u8(); break;
    case SUB_I16:   VM_DEBUG_2("i:SUB_I16");         sub_i16(); break;
    case SUB_U16:   VM_DEBUG_2("i:SUB_U16");         sub_u16(); break;
    case SUB_I32:   VM_DEBUG_2("i:SUB_I32");         sub_i32(); break;
    case SUB_U32:   VM_DEBUG_2("i:SUB_U32");         sub_u32(); break;
    case SUB_F32:   VM_DEBUG_2("i:SUB_F32");         sub_f32(); break;
    case SUB_I64:   VM_DEBUG_2("i:SUB_I64");         sub_i64(); break;
    case SUB_U64:   VM_DEBUG_2("i:SUB_U64");         sub_u64(); break;
    case SUB_F64:   VM_DEBUG_2("i:SUB_F64");         sub_f64(); break;
    
    case MUL_I8:    VM_DEBUG_2("i:MUL_I8");          mul_i8(); break;
    case MUL_U8:    VM_DEBUG_2("i:MUL_U8");          mul_u8(); break;
    case MUL_I16:   VM_DEBUG_2("i:MUL_I16");         mul_i16(); break;
    case MUL_U16:   VM_DEBUG_2("i:MUL_U16");         mul_u16(); break;
    case MUL_I32:   VM_DEBUG_2("i:MUL_I32");         mul_i32(); break;
    case MUL_U32:   VM_DEBUG_2("i:MUL_U32");         mul_u32(); break;
    case MUL_F32:   VM_DEBUG_2("i:MUL_F32");         mul_f32(); break;
    case MUL_I64:   VM_DEBUG_2("i:MUL_I64");         mul_i64(); break;
    case MUL_U64:   VM_DEBUG_2("i:MUL_U64");         mul_u64(); break;
    case MUL_F64:   VM_DEBUG_2("i:MUL_F64");         mul_f64(); break;
    
    case DIV_I8:    VM_DEBUG_2("i:DIV_I8");          div_i8(); break;
    case DIV_U8:    VM_DEBUG_2("i:DIV_U8");          div_u8(); break;
    case DIV_I16:   VM_DEBUG_2("i:DIV_I16");         div_i16(); break;
    case DIV_U16:   VM_DEBUG_2("i:DIV_U16");         div_u16(); break;
    case DIV_I32:   VM_DEBUG_2("i:DIV_I32");         div_i32(); break;
    case DIV_U32:   VM_DEBUG_2("i:DIV_U32");         div_u32(); break;
    case DIV_F32:   VM_DEBUG_2("i:DIV_F32");         div_f32(); break;
    case DIV_I64:   VM_DEBUG_2("i:DIV_I64");         div_i64(); break;
    case DIV_U64:   VM_DEBUG_2("i:DIV_U64");         div_u64(); break;
    case DIV_F64:   VM_DEBUG_2("i:DIV_F64");         div_f64(); break;
    
    case REM_I8:    VM_DEBUG_2("i:REM_I8");          rem_i8(); break;
    case REM_U8:    VM_DEBUG_2("i:REM_U8");          rem_u8(); break;
    case REM_I16:   VM_DEBUG_2("i:REM_I16");         rem_i16(); break;
    case REM_U16:   VM_DEBUG_2("i:REM_U16");         rem_u16(); break;
    case REM_I32:   VM_DEBUG_2("i:REM_I32");         rem_i32(); break;
    case REM_U32:   VM_DEBUG_2("i:REM_U32");         rem_u32(); break;
    case REM_F32:   VM_DEBUG_2("i:REM_F32");         rem_f32(); break;
    case REM_I64:   VM_DEBUG_2("i:REM_I64");         rem_i64(); break;
    case REM_U64:   VM_DEBUG_2("i:REM_U64");         rem_u64(); break;
    case REM_F64:   VM_DEBUG_2("i:REM_F64");         rem_f64(); break;
    
    case ABS_I8:    VM_DEBUG_2("i:ABS_I8");          abs_i8(); break;
    case ABS_I16:   VM_DEBUG_2("i:ABS_I16");         abs_i16(); break;
    case ABS_I32:   VM_DEBUG_2("i:ABS_I32");         abs_i32(); break;
    case ABS_F32:   VM_DEBUG_2("i:ABS_F32");         abs_f32(); break;
    case ABS_I64:   VM_DEBUG_2("i:ABS_I64");         abs_i64(); break;
    case ABS_F64:   VM_DEBUG_2("i:ABS_F64");         abs_f64(); break;
    
    case DEC_I8:    VM_DEBUG_2("i:DEC_I8");          dec_i8(); break;
    case DEC_U8:    VM_DEBUG_2("i:DEC_U8");          dec_u8(); break;
    case DEC_I16:   VM_DEBUG_2("i:DEC_I16");         dec_i16(); break;
    case DEC_U16:   VM_DEBUG_2("i:DEC_U16");         dec_u16(); break;
    case DEC_I32:   VM_DEBUG_2("i:DEC_I32");         dec_i32(); break;
    case DEC_U32:   VM_DEBUG_2("i:DEC_U32");         dec_u32(); break;
    case DEC_F32:   VM_DEBUG_2("i:DEC_F32");         dec_f32(); break;
    case DEC_I64:   VM_DEBUG_2("i:DEC_I64");         dec_i64(); break;
    case DEC_U64:   VM_DEBUG_2("i:DEC_U64");         dec_u64(); break;
    case DEC_F64:   VM_DEBUG_2("i:DEC_F64");         dec_f64(); break;
    
    case INC_I8:    VM_DEBUG_2("i:INC_I8");          inc_i8(); break;
    case INC_U8:    VM_DEBUG_2("i:INC_U8");          inc_u8(); break;
    case INC_I16:   VM_DEBUG_2("i:INC_I16");         inc_i16(); break;
    case INC_U16:   VM_DEBUG_2("i:INC_U16");         inc_u16(); break;
    case INC_I32:   VM_DEBUG_2("i:INC_I32");         inc_i32(); break;
    case INC_U32:   VM_DEBUG_2("i:INC_U32");         inc_u32(); break;
    case INC_F32:   VM_DEBUG_2("i:INC_F32");         inc_f32(); break;
    case INC_I64:   VM_DEBUG_2("i:INC_I64");         inc_i64(); break;
    case INC_U64:   VM_DEBUG_2("i:INC_U64");         inc_u64(); break;
    case INC_F64:   VM_DEBUG_2("i:INC_F64");         inc_f64(); break;

    case DUPG:      VM_DEBUG_2("i:DUPG");            dupg(*(uint8_t *)read(1)); break;
    case DUP1:      VM_DEBUG_2("i:DUP1");            dup1(); break;
    case DUP2:      VM_DEBUG_2("i:DUP2");            dup2(); break;
    case DUP4:      VM_DEBUG_2("i:DUP4");            dup4(); break;
    case DUP8:      VM_DEBUG_2("i:DUP8");            dup8(); break;
    
    case SWAPG:     VM_DEBUG_2("i:SWAPG");           swapg(*(uint8_t *)read(1)); break;
    case SWAP1:     VM_DEBUG_2("i:SWAP1");           swap1(); break;
    case SWAP2:     VM_DEBUG_2("i:SWAP2");           swap2(); break;
    case SWAP4:     VM_DEBUG_2("i:SWAP4");           swap4(); break;
    case SWAP8:     VM_DEBUG_2("i:SWAP8");           swap8(); break;

    case POPG:      VM_DEBUG_2("i:POPG");            popg(*(uint8_t *)read(1)); break;
    case POP1:      VM_DEBUG_2("i:POP1");            pop1(); break;
    case POP2:      VM_DEBUG_2("i:POP2");            pop2(); break;
    case POP4:      VM_DEBUG_2("i:POP4");            pop4(); break;
    case POP8:      VM_DEBUG_2("i:POP8");            pop8(); break;

    case CMP_I8:    VM_DEBUG_2("i:CMP_I8");          cmp_i8(); break;
    case CMP_U8:    VM_DEBUG_2("i:CMP_U8");          cmp_u8(); break;
    case CMP_I16:   VM_DEBUG_2("i:CMP_I16");         cmp_i16(); break;
    case CMP_U16:   VM_DEBUG_2("i:CMP_U16");         cmp_u16(); break;
    case CMP_I32:   VM_DEBUG_2("i:CMP_I32");         cmp_i32(); break;
    case CMP_U32:   VM_DEBUG_2("i:CMP_U32");         cmp_u32(); break;
    case CMP_F32:   VM_DEBUG_2("i:CMP_F32");         cmp_f32(); break;
    case CMP_I64:   VM_DEBUG_2("i:CMP_I64");         cmp_i64(); break;
    case CMP_U64:   VM_DEBUG_2("i:CMP_U64");         cmp_u64(); break;
    case CMP_F64:   VM_DEBUG_2("i:CMP_F64");         cmp_f64(); break;

    case JZ:        VM_DEBUG_2("i:JZ");              jz(*(size_t *)read(8)); break;
    case JNZ:       VM_DEBUG_2("i:JNZ");             jnz(*(size_t *)read(8)); break;
    case JL:        VM_DEBUG_2("i:JL");              jl(*(size_t *)read(8)); break;
    case JG:        VM_DEBUG_2("i:JG");              jg(*(size_t *)read(8)); break;
    case JNL:       VM_DEBUG_2("i:JNL");             jnl(*(size_t *)read(8)); break;
    case JNG:       VM_DEBUG_2("i:JNG");             jng(*(size_t *)read(8)); break;
    case JMP:       VM_DEBUG_2("i:JMP");             jmp(*(size_t *)read(8)); break;

    case RET:       VM_DEBUG_2("i:RET");             ret(); break;

    case DBG:       VM_DEBUG_2("i:DBG");             dbg(*(int8_t *)read(1)); break;
    case SIG:       VM_DEBUG_2("i:SIG");             sig(*(int8_t *)read(1)); break;

    default:
      throw std::runtime_error(fmt::format("Unknown opcode: {}", opcode));
  }

  return true;
}


void VM::push(void *value, size_t size) {
  VM_DEBUG_2("->push {}", size);
  if (stack_top == nullptr) {
    stack_top = stack + size - 1;
    memcpy(stack, value, size);
  } else if (stack_top + size > stack + VM_STACK_SIZE) {
    dbg(-1);
    throw std::runtime_error("push(): stack overflow");
  } else {
    memcpy(stack_top + 1, value, size);
    stack_top += size;
  }
}

void *VM::pop(size_t size) {
  VM_DEBUG_2("->pop {} ({})", size, stack_top - stack);
  if (stack_top == nullptr) {
    dbg(-1);
    throw std::runtime_error("pop(): stack underflow on nullptr");
  } else if (stack_top + 1 - size < stack) {
    dbg(-1);
    throw std::runtime_error("pop(): stack underflow");
  } else if ( stack_top + 1 - size == stack ) {
    stack_top = nullptr;
  } else {
    stack_top -= size;
  }
  return stack_top + 1; // TODO: check if this is correct
}

void *VM::ref(size_t offset) {
  VM_DEBUG_2("->ref {}", offset);
  if (stack_top == nullptr) {
    dbg(-1);
    throw std::runtime_error("ref(): stack underflow on nullptr");
  } else if (stack_top + 1 - offset < stack) {
    dbg(-1);
    throw std::runtime_error("ref(): stack underflow");
  } else {
    return stack_top - offset + 1;
  }
}

void VM::push_i8(int8_t value) { push(&value, sizeof(value)); }
void VM::push_u8(uint8_t value) { push(&value, sizeof(value)); }
void VM::push_bool(bool value) { push(&value, sizeof(value)); }
void VM::push_i16(int16_t value) { push(&value, sizeof(value)); }
void VM::push_u16(uint16_t value) { push(&value, sizeof(value)); }
void VM::push_i32(int32_t value) { push(&value, sizeof(value)); }
void VM::push_u32(uint32_t value) { push(&value, sizeof(value)); }
void VM::push_f32(float value) { push(&value, sizeof(value)); }
void VM::push_i64(int64_t value) { push(&value, sizeof(value)); }
void VM::push_u64(uint64_t value) { push(&value, sizeof(value)); }
void VM::push_f64(double value) { push(&value, sizeof(value)); }



void VM::popl_i8(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_i8 {}", index); scope->local(index, *(int8_t *)pop(sizeof(int8_t))); }
void VM::popl_u8(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_u8 {}", index); scope->local(index, *(uint8_t *)pop(sizeof(uint8_t))); }
void VM::popl_bool(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_bool {}", index); scope->local(index, *(bool *)pop(sizeof(bool))); }
void VM::popl_i16(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_i16 {}", index); scope->local(index, *(int16_t *)pop(sizeof(int16_t))); }
void VM::popl_u16(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_u16 {}", index); scope->local(index, *(uint16_t *)pop(sizeof(uint16_t))); }
void VM::popl_i32(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_i32 {}", index); scope->local(index, *(int32_t *)pop(sizeof(int32_t))); }
void VM::popl_u32(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_u32 {}", index); scope->local(index, *(uint32_t *)pop(sizeof(uint32_t))); }
void VM::popl_f32(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_f32 {}", index); scope->local(index, *(float *)pop(sizeof(float))); }
void VM::popl_i64(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_i64 {}", index); scope->local(index, *(int64_t *)pop(sizeof(int64_t))); }
void VM::popl_u64(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_u64 {}", index); scope->local(index, *(uint64_t *)pop(sizeof(uint64_t))); }
void VM::popl_f64(VMScope *scope, uint8_t index) { VM_DEBUG_2("popl_f64 {}", index); scope->local(index, *(double *)pop(sizeof(double))); }



void VM::pushl_i8(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_i8 {}", index); int8_t value = scope->local(index)->as_i8_safe(); push(&value, sizeof(value)); }
void VM::pushl_u8(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_u8 {}", index); uint8_t value = scope->local(index)->as_u8_safe(); push(&value, sizeof(value)); }
void VM::pushl_bool(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_bool {}", index); bool value = scope->local(index)->as_bool_safe(); push(&value, sizeof(value)); }
void VM::pushl_i16(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_i16 {}", index); int16_t value = scope->local(index)->as_i16_safe(); push(&value, sizeof(value)); }
void VM::pushl_u16(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_u16 {}", index); uint16_t value = scope->local(index)->as_u16_safe(); push(&value, sizeof(value)); }
void VM::pushl_i32(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_i32 {}", index); int32_t value = scope->local(index)->as_i32_safe(); push(&value, sizeof(value)); }
void VM::pushl_u32(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_u32 {}", index); uint32_t value = scope->local(index)->as_u32_safe(); push(&value, sizeof(value)); }
void VM::pushl_f32(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_f32 {}", index); float value = scope->local(index)->as_f32_safe(); push(&value, sizeof(value)); }
void VM::pushl_i64(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_i64 {}", index); int64_t value = scope->local(index)->as_i64_safe(); push(&value, sizeof(value)); }
void VM::pushl_u64(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_u64 {}", index); uint64_t value = scope->local(index)->as_u64_safe(); push(&value, sizeof(value)); }
void VM::pushl_f64(VMScope *scope, uint8_t index) { VM_DEBUG_2("pushl_f64 {}", index); double value = scope->local(index)->as_f64_safe(); push(&value, sizeof(value)); }



void VM::setl_i8(int8_t value, VMScope *scope, uint8_t index)        { VM_DEBUG_2("setl_i8 #{} = {}", index, value);   *scope->local(index) = value; }
void VM::setl_u8(uint8_t value, VMScope *scope, uint8_t index)       { VM_DEBUG_2("setl_u8 #{} = {}", index, value);   *scope->local(index) = value; }
void VM::setl_bool(bool value, VMScope *scope, uint8_t index)        { VM_DEBUG_2("setl_bool #{} = {}", index, value); *scope->local(index) = value; }
void VM::setl_i16(int16_t value, VMScope *scope, uint8_t index)      { VM_DEBUG_2("setl_i16 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_u16(uint16_t value, VMScope *scope, uint8_t index)     { VM_DEBUG_2("setl_u16 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_i32(int32_t value, VMScope *scope, uint8_t index)      { VM_DEBUG_2("setl_i32 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_u32(uint32_t value, VMScope *scope, uint8_t index)     { VM_DEBUG_2("setl_u32 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_f32(float value, VMScope *scope, uint8_t index)        { VM_DEBUG_2("setl_f32 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_i64(int64_t value, VMScope *scope, uint8_t index)      { VM_DEBUG_2("setl_i64 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_u64(uint64_t value, VMScope *scope, uint8_t index)     { VM_DEBUG_2("setl_u64 #{} = {}", index, value);  *scope->local(index) = value; }
void VM::setl_f64(double value, VMScope *scope, uint8_t index)       { VM_DEBUG_2("setl_f64 #{} = {}", index, value);  *scope->local(index) = value; }



#define VM_IMPL_ADD(type, native_type) \
  void VM::add_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("add_##type {} + {} = {}", *a, *b, *a + *b); \
    *b = *a + *b; \
  }

VM_IMPL_ADD(i8, int8_t)
VM_IMPL_ADD(u8, uint8_t)
VM_IMPL_ADD(i16, int16_t)
VM_IMPL_ADD(u16, uint16_t)
VM_IMPL_ADD(i32, int32_t)
VM_IMPL_ADD(u32, uint32_t)
VM_IMPL_ADD(f32, float)
VM_IMPL_ADD(i64, int64_t)
VM_IMPL_ADD(u64, uint64_t)
VM_IMPL_ADD(f64, double)

#undef VM_IMPL_ADD



#define VM_IMPL_SUB(type, native_type) \
  void VM::sub_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("sub_##type {} - {} = {}", *a, *b, *a - *b); \
    *b = *a - *b; \
  }

VM_IMPL_SUB(i8, int8_t)
VM_IMPL_SUB(u8, uint8_t)
VM_IMPL_SUB(i16, int16_t)
VM_IMPL_SUB(u16, uint16_t)
VM_IMPL_SUB(i32, int32_t)
VM_IMPL_SUB(u32, uint32_t)
VM_IMPL_SUB(f32, float)
VM_IMPL_SUB(i64, int64_t)
VM_IMPL_SUB(u64, uint64_t)
VM_IMPL_SUB(f64, double)

#undef VM_IMPL_SUB



#define VM_IMPL_MUL(type, native_type) \
  void VM::mul_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("mul_##type {} * {} = {}", *a, *b, *a * *b); \
    *b = *a * *b; \
  }

VM_IMPL_MUL(i8, int8_t)
VM_IMPL_MUL(u8, uint8_t)
VM_IMPL_MUL(i16, int16_t)
VM_IMPL_MUL(u16, uint16_t)
VM_IMPL_MUL(i32, int32_t)
VM_IMPL_MUL(u32, uint32_t)
VM_IMPL_MUL(f32, float)
VM_IMPL_MUL(i64, int64_t)
VM_IMPL_MUL(u64, uint64_t)
VM_IMPL_MUL(f64, double)

#undef VM_IMPL_MUL



#define VM_IMPL_DIV(type, native_type) \
  void VM::div_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    if (*b == 0) { \
      VM_DEBUG_2("div_##type {} / 0 = undefined", *a); \
      reg_err = 1; \
    } else { \
      VM_DEBUG_2("div_##type {} / {} = {}", *a, *b, *a / *b); \
      *b = *a / *b; \
    } \
  }

VM_IMPL_DIV(i8, int8_t)
VM_IMPL_DIV(u8, uint8_t)
VM_IMPL_DIV(i16, int16_t)
VM_IMPL_DIV(u16, uint16_t)
VM_IMPL_DIV(i32, int32_t)
VM_IMPL_DIV(u32, uint32_t)
VM_IMPL_DIV(f32, float)
VM_IMPL_DIV(i64, int64_t)
VM_IMPL_DIV(u64, uint64_t)
VM_IMPL_DIV(f64, double)

#undef VM_IMPL_DIV



#define VM_IMPL_REM(type, native_type) \
  void VM::rem_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    if (*b == 0) { \
      VM_DEBUG_2("rem_##type {} % 0 = undefined", *a); \
      reg_err = 1; \
    }  else { \
      VM_DEBUG_2("rem_##type {} % {} = {}", *a, *b, *a % *b); \
      *b = *a % *b; \
    } \
  }

#define VM_IMPL_REM_FLOAT(type, native_type, fmodfn) \
  void VM::rem_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    if (*b == 0.0) { \
      VM_DEBUG_2("rem_##type {} % 0.0 = undefined", *a); \
      reg_err = 1; \
    } else { \
      VM_DEBUG_2("rem_##type {} % {} = {}", *a, *b, fmodfn(*a, *b)); \
      *b = fmodfn(*a, *b); \
    } \
  }

VM_IMPL_REM(i8, int8_t)
VM_IMPL_REM(u8, uint8_t)
VM_IMPL_REM(i16, int16_t)
VM_IMPL_REM(u16, uint16_t)
VM_IMPL_REM(i32, int32_t)
VM_IMPL_REM(u32, uint32_t)
VM_IMPL_REM_FLOAT(f32, float, fmodf32)
VM_IMPL_REM(i64, int64_t)
VM_IMPL_REM(u64, uint64_t)
VM_IMPL_REM_FLOAT(f64, double, fmodf64)

#undef VM_IMPL_REM
#undef VM_IMPL_REM_FLOAT



#define VM_IMPL_ABS(type, native_type) \
  void VM::abs_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("abs_##type {} = {}", *a, (*a > 0) ? *a : -*a); \
    *a = (*a > 0) ? *a : -*a; \
  }

VM_IMPL_ABS(i8, int8_t)
VM_IMPL_ABS(i16, int16_t)
VM_IMPL_ABS(i32, int32_t)
VM_IMPL_ABS(f32, float)
VM_IMPL_ABS(i64, int64_t)
VM_IMPL_ABS(f64, double)

#undef VM_IMPL_ABS



#define VM_IMPL_DEC(type, native_type) \
  void VM::dec_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("dec_##type {} = {}", *a, *a - 1); \
    *a = *a - 1; \
  }

VM_IMPL_DEC(i8, int8_t)
VM_IMPL_DEC(u8, uint8_t)
VM_IMPL_DEC(i16, int16_t)
VM_IMPL_DEC(u16, uint16_t)
VM_IMPL_DEC(i32, int32_t)
VM_IMPL_DEC(u32, uint32_t)
VM_IMPL_DEC(f32, float)
VM_IMPL_DEC(i64, int64_t)
VM_IMPL_DEC(u64, uint64_t)
VM_IMPL_DEC(f64, double)

#undef VM_IMPL_DEC



#define VM_IMPL_INC(type, native_type) \
  void VM::inc_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("inc_##type {} = {}", *a, *a + 1); \
    *a = *a + 1; \
  }

VM_IMPL_INC(i8, int8_t)
VM_IMPL_INC(u8, uint8_t)
VM_IMPL_INC(i16, int16_t)
VM_IMPL_INC(u16, uint16_t)
VM_IMPL_INC(i32, int32_t)
VM_IMPL_INC(u32, uint32_t)
VM_IMPL_INC(f32, float)
VM_IMPL_INC(i64, int64_t)
VM_IMPL_INC(u64, uint64_t)
VM_IMPL_INC(f64, double)

#undef VM_IMPL_INC



void VM::dupg(uint8_t n) {
  void *start = ref(n);
  push(start, n);
}

void VM::dup1() { dupg(1); }
void VM::dup2() { dupg(2); }
void VM::dup4() { dupg(4); }
void VM::dup8() { dupg(8); }



void VM::swapg(uint8_t n) {
  void *a = ref(n);
  void *b = ref(n + n);
  void *tmp = malloc(n);
  // swap using memcpy and memmove
  memcpy(tmp, a, n);
  memmove(a, b, n);
  memmove(b, tmp, n);
  free(tmp);
}

void VM::swap1() { swapg(1); }
void VM::swap2() { swapg(2); }
void VM::swap4() { swapg(4); }
void VM::swap8() { swapg(8); }



void VM::popg(uint8_t n) {
  pop(n);
}

void VM::pop1() { popg(1); }
void VM::pop2() { popg(2); }
void VM::pop4() { popg(4); }
void VM::pop8() { popg(8); }



#define VM_IMPL_CMP(type, native_type) \
  void VM::cmp_##type() { \
    native_type *a = (native_type *)ref(sizeof(native_type) + sizeof(native_type)); \
    native_type *b = (native_type *)ref(sizeof(native_type)); \
    VM_DEBUG_2("cmp_##type {} {}", *a, *b); \
    reg_cmp = (*a == *b) ? 0 : ((*a < *b) ? -1 : 1); \
  }

VM_IMPL_CMP(i8, int8_t)
VM_IMPL_CMP(u8, uint8_t)
VM_IMPL_CMP(i16, int16_t)
VM_IMPL_CMP(u16, uint16_t)
VM_IMPL_CMP(i32, int32_t)
VM_IMPL_CMP(u32, uint32_t)
VM_IMPL_CMP(f32, float)
VM_IMPL_CMP(i64, int64_t)
VM_IMPL_CMP(u64, uint64_t)
VM_IMPL_CMP(f64, double)

#undef VM_IMPL_CMP



void VM::jz(size_t offset) {
  VM_DEBUG_1("jz {:#08x} ({})", offset, reg_cmp == 0);
  if (reg_cmp == 0) {
    instruction = program + offset;
  }
}
void VM::jnz(size_t offset) {
  VM_DEBUG_1("jnz {:#08x} ({})", offset, reg_cmp != 0);
  if (reg_cmp != 0) {
    instruction = program + offset;
  }
}
void VM::jl(size_t offset) {
  VM_DEBUG_1("jl {:#08x} ({})", offset, reg_cmp == -1);
  if (reg_cmp == -1) {
    instruction = program + offset;
  }
}
void VM::jg(size_t offset) {
  VM_DEBUG_1("jg {:#08x} ({})", offset, reg_cmp == 1);
  if (reg_cmp == 1) {
    instruction = program + offset;
  }
}
void VM::jnl(size_t offset) {
  VM_DEBUG_1("jnl {:#08x} ({})", offset, reg_cmp != -1);
  if (reg_cmp != -1) {
    instruction = program + offset;
  }
}
void VM::jng(size_t offset) {
  VM_DEBUG_1("jng {:#08x} ({})", offset, reg_cmp != 1);
  if (reg_cmp != 1) {
    instruction = program + offset;
  }
}
void VM::jmp(size_t offset) {
  VM_DEBUG_1("jmp {:#08x}", offset);
  instruction = program + offset;
}



void VM::ret() { VM_DEBUG_1("ret: not implemented"); dbg(0); }

void VM::dbg(int8_t i) {
  (void)i; // unused when debugging is disabled
  VM_DEBUG_1("dbg: {} @ {}", i, (size_t)instruction - (size_t)program);
  // Dump stack
  VM_DEBUG_1("\t...\tstack (stack = {:#08x}, stack_top = {:#08x}, delta(stack -> top) = {}):",
    (size_t) &stack, (size_t) stack_top,
     (int64_t) stack_top - (int64_t) &stack
  );
  for (int64_t i = 0; i <= stack_top - stack; i++) {
    VM_DEBUG_1("\t{:#08x}\t{:#02x}\t{}", (size_t) &stack[i], (size_t) stack[i], stack[i]);
  }
  VM_DEBUG_1("\t...\t*stack_top = {}", *stack_top);
  VM_DEBUG_1("\t...\tlocals:");
  for (size_t i = 0; scope.local(i)->get_type() != _none; i++) {
    VM_DEBUG_1("\t#{}: ({}) = {}", i, scope.local(i)->get_type_name(), scope.local(i)->get_value_string());
  }
}

void VM::sig(int8_t i) {
  VM_DEBUG_1("********************************** Received signal: {} ********************************** ", i);
  dbg(i);
  instruction = program + program_size;
}


} // namespace pushle
