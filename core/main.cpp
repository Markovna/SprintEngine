#include "Application.h"
#include <iostream>
#include <functional>
#include "signals/delegate_tests.h"
#include "signals/function_ref.h"

using namespace sprint;

// replace operator new and delete to log allocations
//void* operator new(std::size_t n) {
//    std::cout << n << std::endl;
//    return malloc(n);
//}
//void operator delete(void* p) throw() {
//    free(p);
//}

int main() {
    Application app;
    return app.Run();
}
