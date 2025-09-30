#include "evaluator.h"
#include <stdexcept>
#include <cmath>
#include <typeinfo>
#include <iostream>

double evaluate(ASTNode* node, Environment* env) {
    if (!node) return 0;

    // Number
    if (auto n = dynamic_cast<NumberNode*>(node)) return n->value;

    // Identifier (variable access)
    if (auto id = dynamic_cast<IdentifierNode*>(node)) {
        return env->getVariable(id->name);
    }

    // Binary op
    if (auto b = dynamic_cast<BinaryOpNode*>(node)) {
        double L = evaluate(b->left, env);
        double R = evaluate(b->right, env);
        const std::string &op = b->op;
        if (op == "+") return L + R;
        if (op == "-") return L - R;
        if (op == "*") return L * R;
        if (op == "/") return L / R;
        if (op == "==") return (L == R) ? 1.0 : 0.0;
        if (op == "!=") return (L != R) ? 1.0 : 0.0;
        if (op == "<") return (L < R) ? 1.0 : 0.0;
        if (op == ">") return (L > R) ? 1.0 : 0.0;
        if (op == "<=") return (L <= R) ? 1.0 : 0.0;
        if (op == ">=") return (L >= R) ? 1.0 : 0.0;
        throw std::runtime_error("Unknown binary operator: " + op);
    }

    // Assignment
    if (auto a = dynamic_cast<AssignNode*>(node)) {
        double val = evaluate(a->value, env);
        env->setVariable(a->name, val);
        return val;
    }

    // Expression statement
    if (auto es = dynamic_cast<ExprStmtNode*>(node)) {
        return evaluate(es->expr, env);
    }

    // Block
    if (auto blk = dynamic_cast<BlockNode*>(node)) {
        double last = 0;
        for (auto stmt : blk->statements) {
            last = evaluate(stmt, env);
        }
        return last;
    }

    // If
    if (auto iff = dynamic_cast<IfNode*>(node)) {
        double cond = evaluate(iff->condition, env);
        if (cond != 0.0) return evaluate(iff->thenBlock, env);
        if (iff->elseBlock) return evaluate(iff->elseBlock, env);
        return 0;
    }

    // While
    if (auto wh = dynamic_cast<WhileNode*>(node)) {
        double last = 0;
        while (evaluate(wh->condition, env) != 0.0) {
            last = evaluate(wh->body, env);
        }
        return last;
    }

    // For
    if (auto fr = dynamic_cast<ForNode*>(node)) {
        double last = 0;
        if (fr->init) evaluate(fr->init, env);
        while (true) {
            if (fr->condition) {
                double c = evaluate(fr->condition, env);
                if (c == 0.0) break;
            }
            last = evaluate(fr->body, env);
            if (fr->update) evaluate(fr->update, env);
            // no condition -> infinite loop; user caution
        }
        return last;
    }

    // Function definition
    if (auto fd = dynamic_cast<FuncDefNode*>(node)) {
        env->setFunction(fd->name, fd);
        return 0;
    }

    // Function call
    if (auto fc = dynamic_cast<FuncCallNode*>(node)) {
        // Look for builtin 'print' (simple)
        if (fc->name == "print") {
            for (size_t i = 0; i < fc->args.size(); ++i) {
                double v = evaluate(fc->args[i], env);
                if (i) std::cout << " ";
                if (std::floor(v) == v) std::cout << (long long)v; else std::cout << v;
            }
            std::cout << std::endl;
            return 0;
        }

        FuncDefNode* func = env->getFunction(fc->name);
        if (func->params.size() != fc->args.size()) throw std::runtime_error("wrong number of args in call to " + fc->name);

        Environment local(env);
        for (size_t i = 0; i < fc->args.size(); ++i) {
            double a = evaluate(fc->args[i], env);
            local.setVariable(func->params[i], a);
        }

        try {
            double r = evaluate(func->body, &local);
            return r;
        } catch (ReturnException &re) {
            return re.value;
        }
    }

    // Return
    if (auto ret = dynamic_cast<ReturnNode*>(node)) {
        double v = 0;
        if (ret->value) v = evaluate(ret->value, env);
        throw ReturnException(v);
    }

    throw std::runtime_error("Unknown AST node in evaluator");
}
