#ifndef LEXER_H
#define LEXER_H

#include <string>

enum class TokenType {
    Number, Identifier,
    Plus, Minus, Mul, Div,
    Assign, Semicolon,
    LParen, RParen,
    LBrace, RBrace,
    If, Else, While, For, Func, Return,
    Comma,
    Less, Greater, LessEq, GreaterEq, Eq, NotEq,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
    std::string text;
    size_t pos;
    char currentChar;
public:
    Lexer(const std::string& src);
    Token getNextToken();
private:
    void advance();
    void skipWhitespace();
    Token number();
    Token identifier();
};

#endif
