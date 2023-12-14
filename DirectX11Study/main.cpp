#include "ExampleApp.h"

#include <vector>
#include <sstream>
#include <list>

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // 디버그 빌드의 경우 실행시점 메모리 점검 기능을 활성화한다.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    AllocConsole();
    HANDLE hc = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    ExampleApp theApp(hInstance);

    if (!theApp.Init())
        return 0;

    return theApp.Run();
}
