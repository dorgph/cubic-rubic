#include "tests/TestDriver.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    const TestDriver driver;
    driver.init();
    return 0;
}
