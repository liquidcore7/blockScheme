#include <iostream>
#include "language/types/expressionFolder.h"

int main() {
    std::string exp("2*3+5-8*14%2");
    std::cout << parser::evalExpression(exp) << std::endl;
    return 0;
}