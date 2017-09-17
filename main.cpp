#include <iostream>
#include "language/types/expressionFolder.h"

int main() {
    std::string exp("-1*228*(5-2^3)");
    std::cout << parser::evalExpression(exp) << std::endl;
    return 0;
}