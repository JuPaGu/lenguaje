#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

struct ASTNode {
    virtual ~ASTNode() {}
};

// Number
struct NumberNode : ASTNode {
    double value;
    NumberNode(double v) : value(v) {}
};

// Identifier (variable name)
struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode(const std::string& n) : name(n) {}
};

// Binary operation (left op right)
struct BinaryOpNode : ASTNode {
    std::string op;
    ASTNode* left;
    ASTNode* right;
    BinaryOpNode(const std::string& o, ASTNode* l, ASTNode* r) : op(o), left(l), right(r) {}
};

// Assignment: name = value
struct AssignNode : ASTNode {
    std::string name;
    ASTNode* value;
    AssignNode(const std::string& n, ASTNode* v) : name(n), value(v) {}
};

// Expression statement wrapper
struct ExprStmtNode : ASTNode {
    ASTNode* expr;
    ExprStmtNode(ASTNode* e) : expr(e) {}
};

// Block of statements
struct BlockNode : ASTNode {
    std::vector<ASTNode*> statements;
};

// If node
struct IfNode : ASTNode {
    ASTNode* condition;
    ASTNode* thenBlock;
    ASTNode* elseBlock; // may be nullptr
    IfNode(ASTNode* c, ASTNode* t, ASTNode* e = nullptr) : condition(c), thenBlock(t), elseBlock(e) {}
};

// While node
struct WhileNode : ASTNode {
    ASTNode* condition;
    ASTNode* body;
    WhileNode(ASTNode* c, ASTNode* b) : condition(c), body(b) {}
};

// For node
struct ForNode : ASTNode {
    ASTNode* init;       // may be nullptr
    ASTNode* condition;  // may be nullptr
    ASTNode* update;     // may be nullptr
    ASTNode* body;
    ForNode(ASTNode* i, ASTNode* c, ASTNode* u, ASTNode* b) : init(i), condition(c), update(u), body(b) {}
};

// Function definition
struct FuncDefNode : ASTNode {
    std::string name;
    std::vector<std::string> params;
    ASTNode* body;
    FuncDefNode(const std::string& n, const std::vector<std::string>& p, ASTNode* b) : name(n), params(p), body(b) {}
};

// Function call
struct FuncCallNode : ASTNode {
    std::string name;
    std::vector<ASTNode*> args;
    FuncCallNode(const std::string& n, const std::vector<ASTNode*>& a) : name(n), args(a) {}
};

// Return
struct ReturnNode : ASTNode {
    ASTNode* value; // may be nullptr
    ReturnNode(ASTNode* v) : value(v) {}
};

#endif
