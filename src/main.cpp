#include <fmt/core.h>
#include <cstdint>

#include "ops.h"
#include "vm.h"

// Calculate the 10th fibonacci number
const uint8_t program[] = {
  // i (on stack)
  lang::Op::PUSH_U8, 0,
  // a(#0) = 0
  lang::Op::SETL_U8, 0, 0,
  // b(#1) = 1
  lang::Op::SETL_U8, 1, 1,
  // temp(#2) = 0
  lang::Op::SETL_U8, 2, 0,
  // start loop
  // if (i == (10 - 1)) goto end
  lang::Op::PUSH_U8, 9,
  lang::Op::CMP_U8,
  lang::Op::POP1,
  lang::Op::JZ, 50,0,0,0,0,0,0,0,
  // i++
  lang::Op::INC_U8,
  // temp(#2) = a(#0) + b(#1)
  lang::Op::PUSHL_U8, 0,
  lang::Op::PUSHL_U8, 1,
  lang::Op::ADD_U8,
  lang::Op::POPL_U8, 2,
  lang::Op::POP1,
  // a(#0) = b(#1)
  lang::Op::PUSHL_U8, 1,
  lang::Op::POPL_U8, 0,
  // b(#1) = temp(#2)
  lang::Op::PUSHL_U8, 2,
  lang::Op::POPL_U8, 1,
  // goto start
  lang::Op::JMP, 11,0,0,0,0,0,0,0,
  // end
  lang::Op::POP1,
  lang::Op::PUSHL_U8, 2, // result
  lang::Op::RET,
};

int main() {
  for (auto op : program)
    fmt::print("{:02x} ", op);
  fmt::print("\nExecuting program...\n");
  lang::VM vm;
  vm.run(program, sizeof(program));
  return 0;
}
