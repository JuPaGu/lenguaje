#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

class Parser {
public:
    Parser(Lexer& lexer);
    ASTNode* parseProgram();

private:
    Lexer& lexer;
    Token cur;

    void advance();
    void expect(TokenType t, const std::string& msg);

    ASTNode* parseStatement();
    ASTNode* parseBlock();
    ASTNode* parseIf();
    ASTNode* parseWhile();
    ASTNode* parseFor();
    ASTNode* parseFuncDef();
    ASTNode* parseReturn();

    ASTNode* parseExpression();
    ASTNode* parseAssignment();
    ASTNode* parseEquality();
    ASTNode* parseRelational();
    ASTNode* parseAdditive();
    ASTNode* parseMultiplicative();
    ASTNode* parseUnary();
    ASTNode* parsePrimary();

    std::vector<ASTNode*> parseCallArgs();
};

#endif
