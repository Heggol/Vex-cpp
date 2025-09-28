#pragma once

#ifdef _WIN32
#include <windows.h>
// Windows setup for correct console output for all characters
inline void setupConsole() {
    SetConsoleOutputCP(CP_UTF8);
}
#else
inline void setupConsole() {
    // Not needed on other platforms
}
#endif
