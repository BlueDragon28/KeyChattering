#ifndef KEYCHATTERING_KEYBOARDHOOK_H_
#define KEYCHATTERING_KEYBOARDHOOK_H_

#include <windows.h>

LRESULT CALLBACK keyHookProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam);

#endif // KEYCHATTERING_KEYBOARDHOOK_H_