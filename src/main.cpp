#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "KeyboardHook.h"
#include "Application.h"

int main(int argc, char** argv)
{
    Application* app = Application::createInstance(argc, argv);
    return app->run();
}