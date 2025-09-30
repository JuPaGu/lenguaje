#include "parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    cur = this->lexer.getNextToken();
}

void Parser::advance() {
    cur = lexer.getNextToken();
}

void Parser::expect(TokenType t, const std::string& msg) {
    if (cur.type != t) throw std::runtime_error("Parse error: " + msg + " (got '" + cur.value + "')");
    advance();
}

ASTNode* Parser::parseProgram() {
    BlockNode* root = new BlockNode();
    while (cur.type != TokenType::EndOfFile) {
        root->statements.push_back(parseStatement());
    }
    return root;
}

ASTNode* Parser::parseStatement() {
    if (cur.type == TokenType::If) return parseIf();
    if (cur.type == TokenType::While) return parseWhile();
    if (cur.type == TokenType::For) return parseFor();
    if (cur.type == TokenType::Func) return parseFuncDef();
    if (cur.type == TokenType::Return) return parseReturn();
    if (cur.type == TokenType::LBrace) return parseBlock();

    // expression or assignment or function call
    ASTNode* expr = parseExpression();
    if (cur.type == TokenType::Semicolon) advance();
    return new ExprStmtNode(expr);
}

ASTNode* Parser::parseBlock() {
    expect(TokenType::LBrace, "expected '{'");
    BlockNode* block = new BlockNode();
    while (cur.type != TokenType::RBrace && cur.type != TokenType::EndOfFile) {
        block->statements.push_back(parseStatement());
    }
    expect(TokenType::RBrace, "expected '}'");
    return block;
}

ASTNode* Parser::parseIf() {
    expect(TokenType::If, "expected if");
    expect(TokenType::LParen, "expected '(' after if");
    ASTNode* cond = parseExpression();
    expect(TokenType::RParen, "expected ')' after if condition");
    ASTNode* thenB = parseStatement();
    ASTNode* elseB = nullptr;
    if (cur.type == TokenType::Else) {
        advance();
        elseB = parseStatement();
    }
    return new IfNode(cond, thenB, elseB);
}

ASTNode* Parser::parseWhile() {
    expect(TokenType::While, "expected while");
    expect(TokenType::LParen, "expected '(' after while");
    ASTNode* cond = parseExpression();
    expect(TokenType::RParen, "expected ')' after while condition");
    ASTNode* body = parseStatement();
    return new WhileNode(cond, body);
}

ASTNode* Parser::parseFor() {
    expect(TokenType::For, "expected for");
    expect(TokenType::LParen, "expected '(' after for");
    ASTNode* init = nullptr;
    if (cur.type != TokenType::Semicolon) init = parseExpression();
    expect(TokenType::Semicolon, "expected ';' after for init");
    ASTNode* cond = nullptr;
    if (cur.type != TokenType::Semicolon) cond = parseExpression();
    expect(TokenType::Semicolon, "expected ';' after for condition");
    ASTNode* update = nullptr;
    if (cur.type != TokenType::RParen) update = parseExpression();
    expect(TokenType::RParen, "expected ')' after for clauses");
    ASTNode* body = parseStatement();
    return new ForNode(init, cond, update, body);
}

ASTNode* Parser::parseFuncDef() {
    expect(TokenType::Func, "expected func");
    if (cur.type != TokenType::Identifier) throw std::runtime_error("expected function name");
    std::string name = cur.value; advance();
    expect(TokenType::LParen, "expected '(' after func name");
    std::vector<std::string> params;
    if (cur.type != TokenType::RParen) {
        if (cur.type != TokenType::Identifier) throw std::runtime_error("expected parameter name");
        params.push_back(cur.value); advance();
        while (cur.type == TokenType::Comma) {
            advance();
            if (cur.type != TokenType::Identifier) throw std::runtime_error("expected parameter name");
            params.push_back(cur.value); advance();
        }
    }
    expect(TokenType::RParen, "expected ')' after params");
    ASTNode* body = parseBlock();
    return new FuncDefNode(name, params, body);
}

ASTNode* Parser::parseReturn() {
    expect(TokenType::Return, "expected return");
    ASTNode* val = nullptr;
    if (cur.type != TokenType::Semicolon) val = parseExpression();
    if (cur.type == TokenType::Semicolon) advance();
    return new ReturnNode(val);
}

// --- Expressions (precedence)
ASTNode* Parser::parseExpression() {
    return parseAssignment();
}

ASTNode* Parser::parseAssignment() {
    ASTNode* left = parseEquality();
    if (cur.type == TokenType::Assign) {
        // left must be identifier
        if (auto id = dynamic_cast<IdentifierNode*>(left)) {
            advance();
            ASTNode* right = parseAssignment();
            return new AssignNode(id->name, right);
        } else {
            throw std::runtime_error("left side of assignment must be identifier");
        }
    }
    return left;
}

ASTNode* Parser::parseEquality() {
    ASTNode* node = parseRelational();
    while (cur.type == TokenType::Eq || cur.type == TokenType::NotEq) {
        std::string op = cur.value; advance();
        ASTNode* right = parseRelational();
        node = new BinaryOpNode(op, node, right);
    }
    return node;
}

ASTNode* Parser::parseRelational() {
    ASTNode* node = parseAdditive();
    while (cur.type == TokenType::Less || cur.type == TokenType::Greater ||
           cur.type == TokenType::LessEq || cur.type == TokenType::GreaterEq) {
        std::string op = cur.value; advance();
        ASTNode* right = parseAdditive();
        node = new BinaryOpNode(op, node, right);
    }
    return node;
}

ASTNode* Parser::parseAdditive() {
    ASTNode* node = parseMultiplicative();
    while (cur.type == TokenType::Plus || cur.type == TokenType::Minus) {
        std::string op = cur.value; advance();
        ASTNode* right = parseMultiplicative();
        node = new BinaryOpNode(op, node, right);
    }
    return node;
}

ASTNode* Parser::parseMultiplicative() {
    ASTNode* node = parseUnary();
    while (cur.type == TokenType::Mul || cur.type == TokenType::Div) {
        std::string op = cur.value; advance();
        ASTNode* right = parseUnary();
        node = new BinaryOpNode(op, node, right);
    }
    return node;
}

ASTNode* Parser::parseUnary() {
    if (cur.type == TokenType::Plus) { advance(); return parseUnary(); }
    if (cur.type == TokenType::Minus) { advance(); ASTNode* r = parseUnary(); return new BinaryOpNode("-", new NumberNode(0), r); }
    return parsePrimary();
}

ASTNode* Parser::parsePrimary() {
    if (cur.type == TokenType::Number) {
        double v = std::stod(cur.value);
        advance();
        return new NumberNode(v);
    }
    if (cur.type == TokenType::Identifier) {
        std::string name = cur.value;
        advance();
        if (cur.type == TokenType::LParen) {
            // function call
            advance();
            std::vector<ASTNode*> args;
            if (cur.type != TokenType::RParen) {
                args = parseCallArgs();
            }
            expect(TokenType::RParen, "expected ')'");
            return new FuncCallNode(name, args);
        }
        return new IdentifierNode(name);
    }
    if (cur.type == TokenType::LParen) {
        advance();
        ASTNode* e = parseExpression();
        expect(TokenType::RParen, "expected ')'");
        return e;
    }
    throw std::runtime_error("unexpected token in primary: " + cur.value);
}

std::vector<ASTNode*> Parser::parseCallArgs() {
    std::vector<ASTNode*> args;
    args.push_back(parseExpression());
    while (cur.type == TokenType::Comma) {
        advance();
        args.push_back(parseExpression());
    }
    return args;
}
