#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "environment.h"
#include "evaluator.h"

int main() {
    Environment globalEnv;
    std::string line;

    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        if (line == "exit") break;

        try {
            Lexer lexer(line);
            Parser parser(lexer);
            ASTNode* program = parser.parseProgram();
            double result = evaluate(program, &globalEnv);

            std::cout << result << std::endl;
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
