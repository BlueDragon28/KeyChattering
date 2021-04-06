#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "KeyboardHook.h"
#include "Application.h"

int main()
{   
    std::cout << "Program starting!" << std::endl;
    Application* app = Application::createInstance();
    return app->run();
}