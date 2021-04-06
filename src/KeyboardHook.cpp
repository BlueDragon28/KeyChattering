#include "KeyboardHook.h"
#include "KeyPressData.h"
#include <iostream>

/*LRESULT CALLBACK keyHookProc(
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
        PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
        DWORD t;

        if (p->vkCode == 0x51)
        {
            std::cout << "Catching the pressing of the Q key." << std::endl;
            return 1;
        }

        std::cout << "A key has been pressed: key number: " << p->vkCode << std::endl;
    } break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);

        if (p->vkCode == 0x51)
        {
            std::cout << "Catching the releasing of the Q key." << std::endl;
            return 1;
        }

        std::cout << "A key has been released: key number: " << p->vkCode << std::endl;
    break;
    }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}*/

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