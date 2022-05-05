#include <fmt/core.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "registry.h"
#include "ops.h"

class Token {
private:
  bool is_label_;
  bool is_string_;
  std::string label_;
  std::string text_;
public:
  Token() : is_label_(false) {}
  Token(const std::string& text) : Token(text, false) {}
  Token(const std::string& text, bool is_string) {
    if (text.starts_with("@")) {
      is_label_ = true;
      label_ = text.substr(1);
    } else {
      is_label_ = false;
      is_string_ = is_string;
      text_ = text;
    }
  }

  bool is_label() const {
    return is_label_;
  }

  bool is_string() const {
    return is_string_;
  }

  bool is_number() const {
    return !is_label_ && !is_string_ && text_.find_first_not_of("0123456789.") == std::string::npos;
  }

  const std::string& label() const {
    return label_;
  }

  const std::string& text() const {
    return text_;
  }

  std::string to_string() const {
    if (is_label_) {
      return fmt::format("Label: @{}", label_);
    } else if (is_string_) {
      return fmt::format("String: {}", text_);
    } else if (is_number()) {
      return fmt::format("Number: {}", text_);
    } else {
      return fmt::format("Token: {}", text_);
    }
  }
};

class Bytecode {
private:
  bool is_label_;
  uint8_t op_;
  std::string label_;
public:
  Bytecode() : is_label_(false), op_(0xff) {}
  Bytecode(uint8_t op) : is_label_(false), op_(op) {}
  Bytecode(const std::string& label) : is_label_(true), op_(0xff), label_(label) {}

  bool is_label() const {
    return is_label_;
  }

  uint8_t op() const {
    return op_;
  }

  const std::string& label() const {
    return label_;
  }
};

int main(int argc, char** argv) {
  if (argc != 3) {
    fmt::print("Usage: {} <input> <output>\n", argv[0]);
    return 1;
  }
  
  std::ifstream in(argv[1]);
  std::ofstream out(argv[2]);

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(in, line)) {
    lines.push_back(line);
  }

  std::vector<std::vector<Token>> program_tokens;

  for (auto line : lines) {
    // trim
    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
      return !std::isspace(ch);
    }));
    if (line.empty() || line.starts_with("//"))
      continue;
    // if (line.starts_with("@")) {
    //   labels[line.substr(1)] = output.size();
    //   continue;
    // }
    // Split line into tokens, including double-quoted strings
    std::vector<Token> tokens;
    std::string token;
    
    bool in_string = false;
    bool in_escape = false;
    bool in_slash = false;
    bool in_star = false;
    bool in_singleline_comment = false;
    bool in_multiline_comment = false;

    for (auto c : line) {
      if (in_singleline_comment) {
        break;
      } else if (in_multiline_comment) {
        if (c == '*') {
          in_star = true;
        } else if (c == '/' && in_star) {
          in_multiline_comment = false;
          in_star = false;
        } else {
          in_star = false;
        }
      } else if (in_string) {
        if (in_escape) {
          in_escape = false;
        } else if (c == '\\') {
          in_escape = true;
        } else if (c == '"') {
          in_string = false;
          tokens.emplace_back(token, true);
          token.clear();
        } else {
          in_escape = false;
          token += c;
        }
      } else if (in_slash) {
        if (c == '/') {
          in_singleline_comment = true;
        } else if (c == '*') {
          in_multiline_comment = true;
        } else {
          in_slash = false;
          in_singleline_comment = false;
          in_multiline_comment = false;
        }
      } else if (c == '/') {
        in_slash = true;
      } else if (c == '"') {
        in_string = true;
      } else if (c == ' ' || c == '\t') {
        if (!token.empty()) {
          tokens.emplace_back(token);
          token.clear();
        }
      } else {
        token += c;
      }
    }

    if (!token.empty()) {
      tokens.emplace_back(token);
    }

    program_tokens.push_back(tokens);
  }

  for (auto tokens : program_tokens) {
    for (auto token : tokens) {
      // fmt::print("{}\n", token.to_string());
    }
    // fmt::print("\n");
  }

  std::vector<std::vector<Bytecode>> line_bytecode;

  for (auto tokens : program_tokens) {
    std::vector<Bytecode> bytes;
    std::vector<lang::DataType> arg_types;
    // lang::DataType arg_types.push_back(lang::DataType::_none);
    for (auto token : tokens) {
      if (token.is_label()) {
        if (!arg_types.empty() && arg_types.front() != lang::DataType::_u64) {
          throw std::runtime_error("Label can only be used with u64");
        } else if (!arg_types.empty()) {
          arg_types.erase(arg_types.begin());
        }
        bytes.emplace_back(token.label());
      } else if (token.is_string()) {
        // TODO
        throw std::runtime_error("String not implemented");
      } else if (token.is_number()) {
        // pop from front
        lang::DataType type = arg_types.front();
        arg_types.erase(arg_types.begin());
        switch (type) {
          case lang::DataType::_none: {
            throw std::runtime_error("Unexpected number");
          }
          case lang::DataType::_i8: {
            // push i8 to bytes
            bytes.emplace_back(std::stoi(token.text()));
            break;
          }
          case lang::DataType::_u8:
          case lang::DataType::_bool:  {// pretty much the same
            // push u8 to bytes
            bytes.emplace_back(std::stoul(token.text()));
            break;
          }
          case lang::DataType::_i16: {
            // push i16 to bytes (little endian)
            int16_t i16 = std::stoi(token.text());
            bytes.emplace_back(i16 & 0xFF);
            bytes.emplace_back(i16 >> 8);
            break;
          }
          case lang::DataType::_u16: {
            // push u16 to bytes (little endian)
            uint16_t u16 = std::stoul(token.text());
            bytes.emplace_back(u16 & 0xFF);
            bytes.emplace_back(u16 >> 8);
            break;
          }
          case lang::DataType::_i32: {
            // push i32 to bytes (little endian)
            int32_t i32 = std::stoi(token.text());
            bytes.emplace_back(i32 & 0xFF);
            bytes.emplace_back(i32 >> 8);
            bytes.emplace_back(i32 >> 16);
            bytes.emplace_back(i32 >> 24);
            break;
          }
          case lang::DataType::_u32: {
            // push u32 to bytes (little endian)
            uint32_t u32 = std::stoul(token.text());
            bytes.emplace_back(u32 & 0xFF);
            bytes.emplace_back(u32 >> 8);
            bytes.emplace_back(u32 >> 16);
            bytes.emplace_back(u32 >> 24);
            break;
          }
          case lang::DataType::_f32: {
            // push f32 to bytes (little endian)
            float f32 = std::stof(token.text());
            uint32_t u32 = (union { float f; uint32_t u; }){f32}.u;
            bytes.emplace_back(u32 & 0xFF);
            bytes.emplace_back(u32 >> 8);
            bytes.emplace_back(u32 >> 16);
            bytes.emplace_back(u32 >> 24);
            break;
          }
          case lang::DataType::_i64: {
            // push i64 to bytes (little endian)
            int64_t i64 = std::stoll(token.text());
            bytes.emplace_back(i64 & 0xFF);
            bytes.emplace_back(i64 >> 8);
            bytes.emplace_back(i64 >> 16);
            bytes.emplace_back(i64 >> 24);
            bytes.emplace_back(i64 >> 32);
            bytes.emplace_back(i64 >> 40);
            bytes.emplace_back(i64 >> 48);
            bytes.emplace_back(i64 >> 56);
            break;
          }
          case lang::DataType::_u64: {
            // push u64 to bytes (little endian)
            uint64_t u64 = std::stoull(token.text());
            bytes.emplace_back(u64 & 0xFF);
            bytes.emplace_back(u64 >> 8);
            bytes.emplace_back(u64 >> 16);
            bytes.emplace_back(u64 >> 24);
            bytes.emplace_back(u64 >> 32);
            bytes.emplace_back(u64 >> 40);
            bytes.emplace_back(u64 >> 48);
            bytes.emplace_back(u64 >> 56);
            break;
          }
          case lang::DataType::_f64: {
            // push f64 to bytes (little endian)
            double f64 = std::stod(token.text());
            uint64_t u64 = (union { double f; uint64_t u; }){f64}.u;
            bytes.emplace_back(u64 & 0xFF);
            bytes.emplace_back(u64 >> 8);
            bytes.emplace_back(u64 >> 16);
            bytes.emplace_back(u64 >> 24);
            bytes.emplace_back(u64 >> 32);
            bytes.emplace_back(u64 >> 40);
            bytes.emplace_back(u64 >> 48);
            bytes.emplace_back(u64 >> 56);
            break;
          }
          default: {
            throw std::runtime_error("Unknown type");
          }
        }
      } else if (arg_types.size() > 0) {
        throw std::runtime_error("Unexpected token (expected argument)");
      } else {
        std::string text = token.text();
        auto registry = lang::TokenRegistry::getInstance();
        auto it = registry.getToken(text);
        if (it.has_value()) {
          auto token = it.value();
          bytes.emplace_back(token.getOp());
          arg_types = token.getArguments();
        } else {
          throw std::runtime_error("unknown opcode: " + text);
        }
      }
    }
    if (arg_types.size() > 0) {
      fmt::print("{}\n", arg_types.size());
      for (auto& t : arg_types) {
        fmt::print("{}\n", t);
      }
      for (auto& t : tokens) {
        fmt::print("{}\n", t.to_string());
      }
      throw std::runtime_error("unexpected end of instruction");
    }
    line_bytecode.push_back(bytes);
  }

  std::vector<uint8_t> bytecode;

  std::map<std::string, uint64_t> label_map;
  std::map<uint64_t, std::string> label_map_usages;
  for (auto& bytes : line_bytecode) {
    if (bytes.size() == 1 && bytes[0].is_label()) {
      // fmt::print("label DEFINE found: {} with bytecode.size() = {}\n", bytes[0].label(), bytecode.size());
      label_map[bytes[0].label()] = bytecode.size();
      continue;
    }
    for (auto& byte : bytes) {
      if (byte.is_label()) {
        // fmt::print("label USAGE found: {} with bytecode.size() = {}\n", byte.label(), bytecode.size());
        label_map_usages[bytecode.size()] = byte.label();
        for (unsigned int i = 0; i < 8; i++) {
          bytecode.emplace_back(0xff);
        }
      } else {
        bytecode.emplace_back(byte.op());
      }
    }
  }
  for (auto& [label, index] : label_map) {
    // fmt::print("DISCOVER label '@{}' at index {}\n", label, index);
  }
  for (auto& [index, label] : label_map_usages) {
    auto address = label_map[label];
    // fmt::print("REMAP index {}: '@{}' -> {}\n", index, label, address);
    // overwrite line_bytecode with the address as uint64_t little endian
    if (bytecode[index] != 0xff
      || bytecode[index + 1] != 0xff
      || bytecode[index + 2] != 0xff
      || bytecode[index + 3] != 0xff
      || bytecode[index + 4] != 0xff
      || bytecode[index + 5] != 0xff
      || bytecode[index + 6] != 0xff
      || bytecode[index + 7] != 0xff) {
      throw std::runtime_error("label already used at index " + std::to_string(index));
    }
    bytecode[index] = address & 0xff;
    bytecode[index + 1] = (address >> 8) & 0xff;
    bytecode[index + 2] = (address >> 16) & 0xff;
    bytecode[index + 3] = (address >> 24) & 0xff;
    bytecode[index + 4] = (address >> 32) & 0xff;
    bytecode[index + 5] = (address >> 40) & 0xff;
    bytecode[index + 6] = (address >> 48) & 0xff;
    bytecode[index + 7] = (address >> 56) & 0xff;
  }

  out.write(reinterpret_cast<char*>(bytecode.data()), bytecode.size());
  out.close();

  return 0;
}
