#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "environment.h"
#include <exception>

struct ReturnException {
    double value;
    ReturnException(double v) : value(v) {}
};

double evaluate(ASTNode* node, Environment* env);

#endif
