#include <iostream>
#include "language/interpreter/Process.h"

int main()
{
    Process fib("language/example/squareFuncArea.blc");
    fib();
    misc::_cleanup();
    return 0;
}
