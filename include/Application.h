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
    Application();
public:
    ~Application();

    static Application* createInstance();
    static Application* instance();
    bool run();

private:
    void init();
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