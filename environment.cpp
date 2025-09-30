#include "environment.h"
#include <stdexcept>

void Environment::setVariable(const std::string& name, double value) {
    // If variable exists in current scope, overwrite; otherwise create in current scope.
    Environment* env = this;
    // search up to see if exists
    Environment* found = nullptr;
    while (env) {
        if (env->variables.find(name) != env->variables.end()) { found = env; break; }
        env = env->parent;
    }
    if (found) found->variables[name] = value;
    else this->variables[name] = value;
}

double Environment::getVariable(const std::string& name) {
    Environment* env = this;
    while (env) {
        auto it = env->variables.find(name);
        if (it != env->variables.end()) return it->second;
        env = env->parent;
    }
    throw std::runtime_error("Variable not defined: " + name);
}

void Environment::setFunction(const std::string& name, FuncDefNode* func) {
    functions[name] = func;
}

FuncDefNode* Environment::getFunction(const std::string& name) {
    Environment* env = this;
    while (env) {
        auto it = env->functions.find(name);
        if (it != env->functions.end()) return it->second;
        env = env->parent;
    }
    throw std::runtime_error("Function not defined: " + name);
}
