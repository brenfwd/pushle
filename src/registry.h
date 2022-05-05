#pragma once

#include "ops.h"

#include <string>
#include <map>
#include <vector>

namespace lang {

class Token {
private:
  Op op;
  std::string token;
  std::vector<DataType> arguments;

public:
  Token(Op op, std::string token, std::vector<DataType> arguments) : op(op), token(token), arguments(arguments) {}
  inline Op getOp() const {
    return op;
  }
  inline std::string getToken() const {
    return token;
  }
  inline std::vector<DataType> getArguments() const {
    return arguments;
  }
  // TODO: inline execution method
};

class TokenRegistry {
private:
  static TokenRegistry *instance;
public:
  static TokenRegistry& getInstance();

  inline std::optional<Token> const getToken(const std::string &token) const {
    for (const auto &t : tokens) {
      if (t.getToken() == token) {
        return t;
      }
    }
    return std::nullopt;
  }
  
  inline void registerToken(const Op op, const std::string &token, std::vector<DataType> arguments) {
    tokens.push_back(Token(op, token, arguments));
  }
private:
  std::vector<Token> tokens;

  TokenRegistry() = default;

};

}
