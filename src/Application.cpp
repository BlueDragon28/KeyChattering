#include "Application.h"
#include "KeyPressData.h"
#include "KeyboardHook.h"
#include <iostream>

std::unique_ptr<Application> Application::_instance = nullptr;

Application::Application() :
    m_isApplicationRunning(true),
    m_initSuccess(0),
    m_hookID(0),
    m_hookThreadID(0)
{
    init();
}

Application::~Application()
{
    deinit();
}

Application* Application::createInstance()
{
    // If no instance of Application has been created, create a new instance
    // and then return it.
    if (!_instance)
        _instance = std::unique_ptr<Application>(new Application());
    
    return _instance.get();
}

Application* Application::instance()
{
    // return an instance of Application.
    return createInstance();
}

void Application::deinit()
{
    // Send a quit message into the hook thread to ask him to exit.
    // Then unhook.
    if (m_hookID != NULL)
    {
        if (m_hookThreadID != 0)
        {
            PostThreadMessage(m_hookThreadID, WM_QUIT, NULL, NULL);
        }
        if (m_tKeyboardHook.joinable())
            m_tKeyboardHook.join();
        UnhookWindowsHookEx(m_hookID);
    }
    m_hookID = NULL;
    m_hookThreadID = NULL;
    m_isApplicationRunning = false;
}

bool Application::run()
{
    // This is the main loop of the application.
    // It wait until the application need to close.
    if (!m_initSuccess)
        return false;
    
    while (m_isApplicationRunning)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    return true;
}

void Application::init()
{
    // This function initialize the core of the program.
    // It initialize the key data, create the keyhook
    // and create the Ctrl+C signal handler.

    // Initialize the KeyPressData class.
    KeyPressData::createInstance();

    // Create the hook into an another thread.
    m_tKeyboardHook = std::thread(&Application::initAndRunKeyboardHook, this);

    // Create the Ctrl+C signal handler.
    createCtrlCSignalHandler();

    // Wait until the hook has been initialized.
    while (m_initSuccess == 0)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }

    if (m_initSuccess < 0)
    {
        m_isApplicationRunning = false;
        std::cout << "Failed to create the keyboard hook." << std::endl;
    }

    std::cout << "init success!" << std::endl;
}

void Application::initAndRunKeyboardHook()
{
    // This function initialize the hook
    // and the loop is used to get the signal
    // of the key pressed and released.

    // Getting the threadid, it is used to send the quit signal.
    m_hookThreadID = GetCurrentThreadId();

    // Create the hook.
    m_hookID = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        keyHookProc,
        nullptr,
        NULL);

    if (m_hookID == 0)
        m_initSuccess = -1;
    else
        m_initSuccess = 1;

    // Get the signals of the input pressed and released.
    // It will also get the quit signal.
    MSG msg;
    while (GetMessage(&msg, NULL, NULL, NULL) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    m_isApplicationRunning = false;
}

void Application::createCtrlCSignalHandler()
{
    // Create the Ctrl+C signal handler.
    if (!SetConsoleCtrlHandler(ctrlcSignalHandler, TRUE))
        std::cerr << "Error, cannot set ctrl+c handler." << std::endl;
}

BOOL Application::ctrlcSignalHandler(DWORD signal)
{
    // If the signal ask to quit. free all
    // the resources and exit.
    switch (signal)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_BREAK_EVENT:
    {
        instance()->deinit();
    } break;
    }

    return TRUE;
}