#include "lexer.h"

#include <iostream>
#include <map>
#include <string>

namespace dolib {
namespace lexer {
namespace {
char lastch = ' ';
}

int current_token = TOKEN_EOF;
std::string ident_str;
std::string op_str;
double real_value = 0.0;

namespace {
// TODO: split token map to allow extra data (ie, binop precedence)
const std::map<std::string, lexer::Token> token_map = {
  { "do", TOKEN_DO },
  { "if", TOKEN_IF },
  { "elif", TOKEN_ELIF },
  { "else", TOKEN_ELSE },
  { "done", TOKEN_DONE },
  { "while", TOKEN_WHILE },
  { "not", TOKEN_UNOP },
  { "map", TOKEN_BUILTIN },
  { "fold", TOKEN_BUILTIN },
  { "filter", TOKEN_BUILTIN },
  { "zip", TOKEN_BUILTIN },
  { "read", TOKEN_BUILTIN },
  { "write", TOKEN_BUILTIN },
  { "length", TOKEN_BUILTIN },
  { "or", TOKEN_LOGIC },
  { "and", TOKEN_LOGIC },
  { "xor", TOKEN_LOGIC },
  { "=", TOKEN_ASSIGN },
  { "+", TOKEN_ARITH },
  { "-", TOKEN_ARITH },
  { "*", TOKEN_ARITH },
  { "/", TOKEN_ARITH },
  { "<", TOKEN_COMPARE },
  { ">", TOKEN_COMPARE },
  { "<=", TOKEN_COMPARE },
  { ">=", TOKEN_COMPARE },
  { "==", TOKEN_COMPARE },
  { "!=", TOKEN_COMPARE },
  { "stdin", TOKEN_STREAM },
  { "stdout", TOKEN_STREAM },
};

int GetToken() {
  ident_str.clear();
  op_str.clear();
  real_value = 0.0;

  while (isspace(lastch))
    lastch = getchar();

  // ident
  if (isalpha(lastch)) {
    std::string s;
    do {
      s += lastch;
      lastch = getchar();
    } while (isalnum(lastch) || lastch == '_' || lastch == '-');

    if (token_map.count(s)) {
      return token_map.at(s);
    }
    ident_str = s;
    return TOKEN_IDENT;
  }

  // real
  if (isdigit(lastch)) {
    bool has_decimal = false;
    std::string s;
    do {
      s += lastch;
      lastch = getchar();
      if (lastch == '.') {
        if (!has_decimal) has_decimal = true;
        else break;
      }
    } while (isdigit(lastch) || lastch == '.');

    // TODO: error check
    real_value = strtod(s.c_str(), 0);
    return TOKEN_REAL;
  }

  // comment
  if (lastch == '#') {
    do lastch = getchar();
    while (lastch != EOF && lastch != '\n');
    if (lastch != EOF) {
      // eat the newline too
      lastch = getchar();
      return GetToken();
    }
  }

  if (lastch == EOF) return TOKEN_EOF;

  // operators
  if (!isalpha(lastch)) {
    std::string s;
    while (lastch != EOF && !isalnum(lastch) && !isspace(lastch)) {
      s += lastch;
      lastch = getchar();
    }

    if (token_map.count(s)) {
      op_str = s;
      return token_map.at(s);
    }
    return lastch;
  }

  int ch = lastch;
  lastch = getchar();
  return ch;
}
}  // end namespace

void Initialize() { NextToken(); }

int NextToken() { return current_token = GetToken(); }

}  // end namespace lexer
}  // end namespace dolib