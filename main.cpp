#include <iostream>
#include "language/types/expressionFolder.h"

int main() {
    std::string exp("-1^2");
    std::cout << parser::evalExpression(exp) << std::endl;
    return 0;
}