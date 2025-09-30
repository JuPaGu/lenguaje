#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include "ast.h"

class Environment {
public:
    std::unordered_map<std::string, double> variables;
    std::unordered_map<std::string, FuncDefNode*> functions;
    Environment* parent = nullptr;

    Environment(Environment* p = nullptr) : parent(p) {}

    void setVariable(const std::string& name, double value);
    double getVariable(const std::string& name);
    void setFunction(const std::string& name, FuncDefNode* func);
    FuncDefNode* getFunction(const std::string& name);
};

#endif
