#include "lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& src) : text(src), pos(0) {
    currentChar = pos < text.size() ? text[pos] : '\0';
}

void Lexer::advance() {
    pos++;
    currentChar = pos < text.size() ? text[pos] : '\0';
}

void Lexer::skipWhitespace() {
    while (currentChar != '\0' && isspace(static_cast<unsigned char>(currentChar))) advance();
}

Token Lexer::number() {
    std::string result;
    while (currentChar != '\0' && isdigit(static_cast<unsigned char>(currentChar))) {
        result += currentChar;
        advance();
    }
    return {TokenType::Number, result};
}

Token Lexer::identifier() {
    std::string result;
    while (currentChar != '\0' && (isalnum(static_cast<unsigned char>(currentChar)) || currentChar == '_')) {
        result += currentChar;
        advance();
    }
    if (result == "if") return {TokenType::If, result};
    if (result == "else") return {TokenType::Else, result};
    if (result == "while") return {TokenType::While, result};
    if (result == "for") return {TokenType::For, result};
    if (result == "func") return {TokenType::Func, result};
    if (result == "return") return {TokenType::Return, result};
    return {TokenType::Identifier, result};
}

Token Lexer::getNextToken() {
    while (currentChar != '\0') {
        if (isspace(static_cast<unsigned char>(currentChar))) { skipWhitespace(); continue; }

        if (isdigit(static_cast<unsigned char>(currentChar))) return number();
        if (isalpha(static_cast<unsigned char>(currentChar)) || currentChar == '_') return identifier();

        if (currentChar == '+') { advance(); return {TokenType::Plus, "+"}; }
        if (currentChar == '-') { advance(); return {TokenType::Minus, "-"}; }
        if (currentChar == '*') { advance(); return {TokenType::Mul, "*"}; }
        if (currentChar == '/') { advance(); return {TokenType::Div, "/"}; }
        if (currentChar == '=') {
            advance();
            if (currentChar == '=') { advance(); return {TokenType::Eq, "=="}; }
            return {TokenType::Assign, "="};
        }
        if (currentChar == ';') { advance(); return {TokenType::Semicolon, ";"}; }
        if (currentChar == '(') { advance(); return {TokenType::LParen, "("}; }
        if (currentChar == ')') { advance(); return {TokenType::RParen, ")"}; }
        if (currentChar == '{') { advance(); return {TokenType::LBrace, "{"}; }
        if (currentChar == '}') { advance(); return {TokenType::RBrace, "}"}; }
        if (currentChar == ',') { advance(); return {TokenType::Comma, ","}; }
        if (currentChar == '<') {
            advance();
            if (currentChar == '=') { advance(); return {TokenType::LessEq, "<="}; }
            return {TokenType::Less, "<"};
        }
        if (currentChar == '>') {
            advance();
            if (currentChar == '=') { advance(); return {TokenType::GreaterEq, ">="}; }
            return {TokenType::Greater, ">"};
        }
        if (currentChar == '!') {
            advance();
            if (currentChar == '=') { advance(); return {TokenType::NotEq, "!="}; }
            throw std::runtime_error("Unexpected '!'");
        }

        throw std::runtime_error("Unknown character: " + std::string(1, currentChar));
    }
    return {TokenType::EndOfFile, ""};
}
