#include "src/core/Application.h"

namespace
{
    int RunApplication()
    {
        const core::Application application;
        return application.Run();
    }
}

int main()
{
    return RunApplication();
}

#ifdef _WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return RunApplication();
}
#endif
