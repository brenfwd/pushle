#include <fmt/core.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>

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
        if (text == "push_i8") {
          bytes.emplace_back(lang::Op::PUSH_I8);
          arg_types.push_back(lang::DataType::_i8);
        }
        else if (text == "push_u8") {
          bytes.emplace_back(lang::Op::PUSH_U8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "push_bool") {
          bytes.emplace_back(lang::Op::PUSH_BOOL);
          arg_types.push_back(lang::DataType::_bool);
        }
        else if (text == "push_i16") {
          bytes.emplace_back(lang::Op::PUSH_I16);
          arg_types.push_back(lang::DataType::_i16);
        }
        else if (text == "push_u16") {
          bytes.emplace_back(lang::Op::PUSH_U16);
          arg_types.push_back(lang::DataType::_u16);
        }
        else if (text == "push_i32") {
          bytes.emplace_back(lang::Op::PUSH_I32);
          arg_types.push_back(lang::DataType::_i32);
        }
        else if (text == "push_u32") {
          bytes.emplace_back(lang::Op::PUSH_U32);
          arg_types.push_back(lang::DataType::_u32);
        }
        else if (text == "push_f32") {
          bytes.emplace_back(lang::Op::PUSH_F32);
          arg_types.push_back(lang::DataType::_f32);
        }
        else if (text == "push_i64") {
          bytes.emplace_back(lang::Op::PUSH_I64);
          arg_types.push_back(lang::DataType::_i64);
        }
        else if (text == "push_u64") {
          bytes.emplace_back(lang::Op::PUSH_U64);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "push_f64") {
          bytes.emplace_back(lang::Op::PUSH_F64);
          arg_types.push_back(lang::DataType::_f64);
        }
        else if (text == "popl_i8") {
          bytes.emplace_back(lang::Op::POPL_I8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_u8") {
          bytes.emplace_back(lang::Op::POPL_U8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_bool") {
          bytes.emplace_back(lang::Op::POPL_BOOL);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_i16") {
          bytes.emplace_back(lang::Op::POPL_I16);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_u16") {
          bytes.emplace_back(lang::Op::POPL_U16);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_i32") {
          bytes.emplace_back(lang::Op::POPL_I32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_u32") {
          bytes.emplace_back(lang::Op::POPL_U32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_f32") {
          bytes.emplace_back(lang::Op::POPL_F32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_i64") {
          bytes.emplace_back(lang::Op::POPL_I64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_u64") {
          bytes.emplace_back(lang::Op::POPL_U64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "popl_f64") {
          bytes.emplace_back(lang::Op::POPL_F64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_i8") {
          bytes.emplace_back(lang::Op::PUSHL_I8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_u8") {
          bytes.emplace_back(lang::Op::PUSHL_U8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_bool") {
          bytes.emplace_back(lang::Op::PUSHL_BOOL);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_i16") {
          bytes.emplace_back(lang::Op::PUSHL_I16);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_u16") {
          bytes.emplace_back(lang::Op::PUSHL_U16);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_i32") {
          bytes.emplace_back(lang::Op::PUSHL_I32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_u32") {
          bytes.emplace_back(lang::Op::PUSHL_U32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_f32") {
          bytes.emplace_back(lang::Op::PUSHL_F32);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_i64") {
          bytes.emplace_back(lang::Op::PUSHL_I64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_u64") {
          bytes.emplace_back(lang::Op::PUSHL_U64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pushl_f64") {
          bytes.emplace_back(lang::Op::PUSHL_F64);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "setl_i8") {
          bytes.emplace_back(lang::Op::SETL_I8);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_i8);
        }
        else if (text == "setl_u8") {
          bytes.emplace_back(lang::Op::SETL_U8);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "setl_bool") {
          bytes.emplace_back(lang::Op::SETL_BOOL);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_bool);
        }
        else if (text == "setl_i16") {
          bytes.emplace_back(lang::Op::SETL_I16);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_i16);
        }
        else if (text == "setl_u16") {
          bytes.emplace_back(lang::Op::SETL_U16);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_u16);
        }
        else if (text == "setl_i32") {
          bytes.emplace_back(lang::Op::SETL_I32);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_i32);
        }
        else if (text == "setl_u32") {
          bytes.emplace_back(lang::Op::SETL_U32);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_u32);
        }
        else if (text == "setl_f32") {
          bytes.emplace_back(lang::Op::SETL_F32);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_f32);
        }
        else if (text == "setl_i64") {
          bytes.emplace_back(lang::Op::SETL_I64);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_i64);
        }
        else if (text == "setl_u64") {
          bytes.emplace_back(lang::Op::SETL_U64);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "setl_f64") {
          bytes.emplace_back(lang::Op::SETL_F64);
          arg_types.push_back(lang::DataType::_u8);
          arg_types.push_back(lang::DataType::_f64);
        }
        else if (text == "add_i8") {
          bytes.emplace_back(lang::Op::ADD_I8);
        }
        else if (text == "add_u8") {
          bytes.emplace_back(lang::Op::ADD_U8);
        }
        else if (text == "add_i16") {
          bytes.emplace_back(lang::Op::ADD_I16);
        }
        else if (text == "add_u16") {
          bytes.emplace_back(lang::Op::ADD_U16);
        }
        else if (text == "add_i32") {
          bytes.emplace_back(lang::Op::ADD_I32);
        }
        else if (text == "add_u32") {
          bytes.emplace_back(lang::Op::ADD_U32);
        }
        else if (text == "add_f32") {
          bytes.emplace_back(lang::Op::ADD_F32);
        }
        else if (text == "add_i64") {
          bytes.emplace_back(lang::Op::ADD_I64);
        }
        else if (text == "add_u64") {
          bytes.emplace_back(lang::Op::ADD_U64);
        }
        else if (text == "add_f64") {
          bytes.emplace_back(lang::Op::ADD_F64);
        }
        else if (text == "sub_i8") {
          bytes.emplace_back(lang::Op::SUB_I8);
        }
        else if (text == "sub_u8") {
          bytes.emplace_back(lang::Op::SUB_U8);
        }
        else if (text == "sub_i16") {
          bytes.emplace_back(lang::Op::SUB_I16);
        }
        else if (text == "sub_u16") {
          bytes.emplace_back(lang::Op::SUB_U16);
        }
        else if (text == "sub_i32") {
          bytes.emplace_back(lang::Op::SUB_I32);
        }
        else if (text == "sub_u32") {
          bytes.emplace_back(lang::Op::SUB_U32);
        }
        else if (text == "sub_f32") {
          bytes.emplace_back(lang::Op::SUB_F32);
        }
        else if (text == "sub_i64") {
          bytes.emplace_back(lang::Op::SUB_I64);
        }
        else if (text == "sub_u64") {
          bytes.emplace_back(lang::Op::SUB_U64);
        }
        else if (text == "sub_f64") {
          bytes.emplace_back(lang::Op::SUB_F64);
        }
        else if (text == "mul_i8") {
          bytes.emplace_back(lang::Op::MUL_I8);
        }
        else if (text == "mul_u8") {
          bytes.emplace_back(lang::Op::MUL_U8);
        }
        else if (text == "mul_i16") {
          bytes.emplace_back(lang::Op::MUL_I16);
        }
        else if (text == "mul_u16") {
          bytes.emplace_back(lang::Op::MUL_U16);
        }
        else if (text == "mul_i32") {
          bytes.emplace_back(lang::Op::MUL_I32);
        }
        else if (text == "mul_u32") {
          bytes.emplace_back(lang::Op::MUL_U32);
        }
        else if (text == "mul_f32") {
          bytes.emplace_back(lang::Op::MUL_F32);
        }
        else if (text == "mul_i64") {
          bytes.emplace_back(lang::Op::MUL_I64);
        }
        else if (text == "mul_u64") {
          bytes.emplace_back(lang::Op::MUL_U64);
        }
        else if (text == "mul_f64") {
          bytes.emplace_back(lang::Op::MUL_F64);
        }
        else if (text == "div_i8") {
          bytes.emplace_back(lang::Op::DIV_I8);
        }
        else if (text == "div_u8") {
          bytes.emplace_back(lang::Op::DIV_U8);
        }
        else if (text == "div_i16") {
          bytes.emplace_back(lang::Op::DIV_I16);
        }
        else if (text == "div_u16") {
          bytes.emplace_back(lang::Op::DIV_U16);
        }
        else if (text == "div_i32") {
          bytes.emplace_back(lang::Op::DIV_I32);
        }
        else if (text == "div_u32") {
          bytes.emplace_back(lang::Op::DIV_U32);
        }
        else if (text == "div_f32") {
          bytes.emplace_back(lang::Op::DIV_F32);
        }
        else if (text == "div_i64") {
          bytes.emplace_back(lang::Op::DIV_I64);
        }
        else if (text == "div_u64") {
          bytes.emplace_back(lang::Op::DIV_U64);
        }
        else if (text == "div_f64") {
          bytes.emplace_back(lang::Op::DIV_F64);
        }
        else if (text == "rem_i8") {
          bytes.emplace_back(lang::Op::REM_I8);
        }
        else if (text == "rem_u8") {
          bytes.emplace_back(lang::Op::REM_U8);
        }
        else if (text == "rem_i16") {
          bytes.emplace_back(lang::Op::REM_I16);
        }
        else if (text == "rem_u16") {
          bytes.emplace_back(lang::Op::REM_U16);
        }
        else if (text == "rem_i32") {
          bytes.emplace_back(lang::Op::REM_I32);
        }
        else if (text == "rem_u32") {
          bytes.emplace_back(lang::Op::REM_U32);
        }
        else if (text == "rem_f32") {
          bytes.emplace_back(lang::Op::REM_F32);
        }
        else if (text == "rem_i64") {
          bytes.emplace_back(lang::Op::REM_I64);
        }
        else if (text == "rem_u64") {
          bytes.emplace_back(lang::Op::REM_U64);
        }
        else if (text == "rem_f64") {
          bytes.emplace_back(lang::Op::REM_F64);
        }
        else if (text == "abs_i8") {
          bytes.emplace_back(lang::Op::ABS_I8);
        }
        else if (text == "abs_i16") {
          bytes.emplace_back(lang::Op::ABS_I16);
        }
        else if (text == "abs_i32") {
          bytes.emplace_back(lang::Op::ABS_I32);
        }
        else if (text == "abs_f32") {
          bytes.emplace_back(lang::Op::ABS_F32);
        }
        else if (text == "abs_i64") {
          bytes.emplace_back(lang::Op::ABS_I64);
        }
        else if (text == "abs_f64") {
          bytes.emplace_back(lang::Op::ABS_F64);
        }
        else if (text == "dec_i8") {
          bytes.emplace_back(lang::Op::DEC_I8);
        }
        else if (text == "dec_u8") {
          bytes.emplace_back(lang::Op::DEC_U8);
        }
        else if (text == "dec_i16") {
          bytes.emplace_back(lang::Op::DEC_I16);
        }
        else if (text == "dec_u16") {
          bytes.emplace_back(lang::Op::DEC_U16);
        }
        else if (text == "dec_i32") {
          bytes.emplace_back(lang::Op::DEC_I32);
        }
        else if (text == "dec_u32") {
          bytes.emplace_back(lang::Op::DEC_U32);
        }
        else if (text == "dec_f32") {
          bytes.emplace_back(lang::Op::DEC_F32);
        }
        else if (text == "dec_i64") {
          bytes.emplace_back(lang::Op::DEC_I64);
        }
        else if (text == "dec_u64") {
          bytes.emplace_back(lang::Op::DEC_U64);
        }
        else if (text == "dec_f64") {
          bytes.emplace_back(lang::Op::DEC_F64);
        }
        else if (text == "inc_i8") {
          bytes.emplace_back(lang::Op::INC_I8);
        }
        else if (text == "inc_u8") {
          bytes.emplace_back(lang::Op::INC_U8);
        }
        else if (text == "inc_i16") {
          bytes.emplace_back(lang::Op::INC_I16);
        }
        else if (text == "inc_u16") {
          bytes.emplace_back(lang::Op::INC_U16);
        }
        else if (text == "inc_i32") {
          bytes.emplace_back(lang::Op::INC_I32);
        }
        else if (text == "inc_u32") {
          bytes.emplace_back(lang::Op::INC_U32);
        }
        else if (text == "inc_f32") {
          bytes.emplace_back(lang::Op::INC_F32);
        }
        else if (text == "inc_i64") {
          bytes.emplace_back(lang::Op::INC_I64);
        }
        else if (text == "inc_u64") {
          bytes.emplace_back(lang::Op::INC_U64);
        }
        else if (text == "inc_f64") {
          bytes.emplace_back(lang::Op::INC_F64);
        }
        else if (text == "dupg") {
          bytes.emplace_back(lang::Op::DUPG);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "dup1") {
          bytes.emplace_back(lang::Op::DUP1);
        }
        else if (text == "dup2") {
          bytes.emplace_back(lang::Op::DUP2);
        }
        else if (text == "dup4") {
          bytes.emplace_back(lang::Op::DUP4);
        }
        else if (text == "dup8") {
          bytes.emplace_back(lang::Op::DUP8);
        }
        else if (text == "dup16") {
          bytes.emplace_back(lang::Op::DUP16);
        }
        else if (text == "swapg") {
          bytes.emplace_back(lang::Op::SWAPG);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "swap1") {
          bytes.emplace_back(lang::Op::SWAP1);
        }
        else if (text == "swap2") {
          bytes.emplace_back(lang::Op::SWAP2);
        }
        else if (text == "swap4") {
          bytes.emplace_back(lang::Op::SWAP4);
        }
        else if (text == "swap8") {
          bytes.emplace_back(lang::Op::SWAP8);
        }
        else if (text == "swap16") {
          bytes.emplace_back(lang::Op::SWAP16);
        }
        else if (text == "popg") {
          bytes.emplace_back(lang::Op::POPG);
          arg_types.push_back(lang::DataType::_u8);
        }
        else if (text == "pop1") {
          bytes.emplace_back(lang::Op::POP1);
        }
        else if (text == "pop2") {
          bytes.emplace_back(lang::Op::POP2);
        }
        else if (text == "pop4") {
          bytes.emplace_back(lang::Op::POP4);
        }
        else if (text == "pop8") {
          bytes.emplace_back(lang::Op::POP8);
        }
        else if (text == "pop16") {
          bytes.emplace_back(lang::Op::POP16);
        }
        else if (text == "cmp_i8") {
          bytes.emplace_back(lang::Op::CMP_I8);
        }
        else if (text == "cmp_u8") {
          bytes.emplace_back(lang::Op::CMP_U8);
        }
        else if (text == "cmp_i16") {
          bytes.emplace_back(lang::Op::CMP_I16);
        }
        else if (text == "cmp_u16") {
          bytes.emplace_back(lang::Op::CMP_U16);
        }
        else if (text == "cmp_i32") {
          bytes.emplace_back(lang::Op::CMP_I32);
        }
        else if (text == "cmp_u32") {
          bytes.emplace_back(lang::Op::CMP_U32);
        }
        else if (text == "cmp_f32") {
          bytes.emplace_back(lang::Op::CMP_F32);
        }
        else if (text == "cmp_i64") {
          bytes.emplace_back(lang::Op::CMP_I64);
        }
        else if (text == "cmp_u64") {
          bytes.emplace_back(lang::Op::CMP_U64);
        }
        else if (text == "cmp_f64") {
          bytes.emplace_back(lang::Op::CMP_F64);
        }
        else if (text == "jz") {
          bytes.emplace_back(lang::Op::JZ);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jnz") {
          bytes.emplace_back(lang::Op::JNZ);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jl") {
          bytes.emplace_back(lang::Op::JL);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jg") {
          bytes.emplace_back(lang::Op::JG);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jnl") {
          bytes.emplace_back(lang::Op::JNL);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jng") {
          bytes.emplace_back(lang::Op::JNG);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "jmp") {
          bytes.emplace_back(lang::Op::JMP);
          arg_types.push_back(lang::DataType::_u64);
        }
        else if (text == "ret") {
          bytes.emplace_back(lang::Op::RET);
        }
        else if (text == "dbg") {
          bytes.emplace_back(lang::Op::DBG);
          arg_types.push_back(lang::DataType::_i8);
        }
        else if (text == "sig") {
          bytes.emplace_back(lang::Op::SIG);
          arg_types.push_back(lang::DataType::_i8);
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
      fmt::print("label DEFINE found: {} with bytecode.size() = {}\n", bytes[0].label(), bytecode.size());
      label_map[bytes[0].label()] = bytecode.size();
      continue;
    }
    for (auto& byte : bytes) {
      if (byte.is_label()) {
        fmt::print("label USAGE found: {} with bytecode.size() = {}\n", byte.label(), bytecode.size());
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
    fmt::print("DISCOVER label '@{}' at index {}\n", label, index);
  }
  for (auto& [index, label] : label_map_usages) {
    auto address = label_map[label];
    fmt::print("REMAP index {}: '@{}' -> {}\n", index, label, address);
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

  fmt::print("done\n");

  return 0;
}
