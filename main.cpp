#include <iostream>
#include "language/interpreter/Process.h"

int main()
{
    Process fib("/home/liquidcore7/CLionProjects/blockScheme/language/example/squareFuncArea.blc");
    fib();
    misc::_cleanup();
    return 0;
}