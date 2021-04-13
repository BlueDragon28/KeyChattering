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

#ifndef KEYCHATTERING_APPLICATION_H_
#define KEYCHATTERING_APPLICATION_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include <windows.h>

class Application
{
    Application(const Application&) = delete;
    Application(int &argc, char**& argv);
public:
    ~Application();

    static Application* createInstance();
    static Application* createInstance(int& argc, char**& argv);
    static Application* instance();
    bool run();

private:
    void init(int& argc, char**& argv);
    void deinit();
    void initAndRunKeyboardHook();
    void createCtrlCSignalHandler();
    static BOOL WINAPI ctrlcSignalHandler(DWORD signal);

    std::atomic<bool> m_isApplicationRunning;
    std::atomic<int> m_initSuccess;
    std::thread m_tKeyboardHook;
    std::atomic<HHOOK> m_hookID;
    std::atomic<DWORD> m_hookThreadID;

    static std::unique_ptr<Application> _instance;
};

#endif // KEYCHATTERING_APPLICATION_H_