#include "KeyboardHook.h"
#include "KeyPressData.h"
#include <iostream>

LRESULT CALLBACK keyHookProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    
    switch (wParam)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            bool result = KeyPressData::instance()->isKeyPressChatter(p->vkCode);
            if (result)
                return 1;
        } break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            bool result = KeyPressData::instance()->isKeyReleaseChatter(p->vkCode);
            if (result)
                return 1;
        } break;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}