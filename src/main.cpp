#include <fmt/core.h>
#include <cstdint>

#include "ops.h"
#include "vm.h"

/*

subroutine fib() returns u64
  local a: u64 = 0    // local#0
  local b: u64 = 1    // local#1
  local temp: u64 = 0 // local#2

  stack i: u8 = 0
%label1
  if i >= 9 goto label2
    i++
    temp = a + b
    a = b
    b = temp
    goto label1
%label2
  discard i

  return temp
end subroutine


*/


// Calculate the 10th fibonacci number
const uint8_t program[] = {
  // stack i: u16 = 0
  lang::Op::PUSH_U16,    0,0,
  // local a: u128 = 0
  lang::Op::SETL_U128,  0,    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  // local b: u128 = 1
  lang::Op::SETL_U128,  1,    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  // local temp: u128 = 0
  lang::Op::SETL_U128,  2,    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  // start loop
  // if (i >= 100-1) goto end
  lang::Op::PUSH_U16,    0xaa, 0x00, //100-1,0,
  lang::Op::CMP_U16,
  lang::Op::POP2,
  lang::Op::JNL,        97,0,0,0,0,0,0,0,
  // i++
  lang::Op::INC_U16,
  // temp = a + b
  lang::Op::PUSHL_U128, 0, // push from a
  lang::Op::PUSHL_U128, 1, // push from b
  lang::Op::ADD_U128,
  lang::Op::POPL_U128,  2, // pop to temp
  lang::Op::POP16,          // discard pushed a
  // a = b
  lang::Op::PUSHL_U128, 1, // push from b
  lang::Op::POPL_U128,  0, // pop to a
  // b = temp
  lang::Op::PUSHL_U128, 2, // push from temp
  lang::Op::POPL_U128,  1, // pop to b
  // goto start
  lang::Op::JMP,        57,0,0,0,0,0,0,0,
  // end
  lang::Op::POP2,          // discard i
  lang::Op::PUSHL_U128, 2, // push from temp
  lang::Op::RET,
};

/*

=== source: ===

u8 fib() {
  u8 a = 0, b = 1;
  for (u8 i = 0; i < 9; i++) {
    u8 temp = a + b;
    a = b;
    b = temp;
  }
  return b;
}



=== optimizer: ===

subroutine fib() returns u8
  local a: u8 = 0
  local b: u8 = 1
  local temp: u8 = 0

  stack i: u8 = 0
%label1
  if i >= 9 goto label2
    i++
    temp = a + b
    a = b
    b = temp
    goto label1
%label2
  discard i

  return temp
end subroutine



=== compiler: ===

decl_subroutine "fib" type::u8
  push_u8 0               // stack i: u8 = 0
  setl_u8 0 0             // local a: u8 = 0
  setl_u8 1 1             // local b: u8 = 1
  setl_u8 2 0             // local temp: u8 = 0
                          // if (i >= 9)
  push_u8 9               // ... rhs for compare (lhs is on stack)
  cmp_u8                  // ... compare (>=)
  pop1                    // ... discard 
  jnl [50]                // ... if true, goto label2
  inc_u8                  // increment i
                          // temp = a + b
  pushl_u8 0              // ... lhs for add
  pushl_u8 1              // ... rhs for add
  add_u8                  // ... add
  popl_u8 2               // ... pop result to temp local
  pop1                    // ... discard lhs
                          // a = b
  pushl_u8 1              // ... push b to stack
  popl_u8 0               // ... pop b to a
                          // b = temp
  pushl_u8 2              // ... push temp to stack
  popl_u8 1               // ... pop temp to b
  jmp [11]                // goto label1
  pop1                    // discard i
                          // return temp
  pushl_u8 2              // ... push temp to stack
  ret                     // ... return
end_subroutine
*/

int main() {
  for (auto op : program)
    fmt::print("{:02x} ", op);
  fmt::print("\nExecuting program...\n");
  lang::VM vm;
  vm.run(program, sizeof(program));
  fmt::print("Result as u128: {}\n", boost::multiprecision::to_string(vm.get_u128()));
  return 0;
}
