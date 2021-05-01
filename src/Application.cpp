/*
* MIT Licence
*
* KeyChattering
*
* Copyright © 2021 Erwan Saclier de la Bâtie (Erwan28250)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Application.h"
#include "KeyPressData.h"
#include "KeyboardHook.h"
#include "CommandLineParsing.h"
#include <iostream>

std::unique_ptr<Application> Application::_instance = nullptr;

Application::Application(int &argc, char**& argv) :
    m_isApplicationRunning(true),
    m_initSuccess(0),
    m_hookID(0),
    m_hookThreadID(0)
{
    init(argc, argv);
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
    {
        int argc = 0;
        char** argv = nullptr;
        return createInstance(argc, argv);
    }
    
    return _instance.get();
}

Application* Application::createInstance(int& argc, char**& argv)
{
    // If no instance of Application has been created, create a new instance
    // and then return it.
    if (!_instance)
        _instance = std::unique_ptr<Application>(new Application(argc, argv));
    
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
    {
        KeyPressData::instance()->removingFinishedThread();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    return true;
}

void Application::init(int& argc, char**& argv)
{
    // This function initialize the core of the program.
    // It parse the command line, initialize the key data, create the keyhook
    // and create the Ctrl+C signal handler.

    // Parsing the command line.
    CommandLineParsing cmdParsing(argc, argv);

    // Initialize the KeyPressData class.
    KeyPressData::createInstance();

    // Set the time of chatter if treated in the command line arguments.
    if (cmdParsing.isMSecSet())
        KeyPressData::instance()->setChatterTime(cmdParsing.msec());

    // Enable debug.
    bool debug = false;
    if (cmdParsing.isDebugSet())
    {
        KeyPressData::instance()->enableDebug(true);
        debug = true;
    }

    std::cout << "Program starting!" << std::endl;

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
        KeyPressData::instance()->waitForThreadToFinish();
    } break;
    }

    return TRUE;
}