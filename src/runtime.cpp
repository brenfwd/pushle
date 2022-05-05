#include <fmt/core.h>
#include <cstdint>

#include <fstream>

#include "ops.h"
#include "pushle.h"

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
  pushle::VM vm;
  vm.run(program.data(), program.size());
  fmt::print("Result as u64: {}\n", vm.get_u64());
  return 0;
}
