#include <fmt/core.h>
#include <cstdint>

#include <fstream>

#include "ops.h"
#include "vm.h"

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

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print("Usage: {} <file>\n", argv[0]);
    return 1;
  }
  std::ifstream ifs(argv[1]);
  if (!ifs) {
    fmt::print("Failed to open file: {}\n", argv[1]);
    return 1;
  }
  std::vector<uint8_t> program;
  uint8_t byte;
  while (ifs.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
    program.push_back(byte);
  }
  lang::VM vm;
  vm.run(program.data(), program.size());
  fmt::print("Result as u64: {}\n", vm.get_u64());
  return 0;
}
