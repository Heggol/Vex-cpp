#pragma once

#ifdef _WIN32
#include <windows.h>
// Windows setup for correct console output for all characters
inline void setup_console() {
    SetConsoleOutputCP(CP_UTF8);
}
#else
inline void setup_console() {
    // Not needed on other platforms
}
#endif
