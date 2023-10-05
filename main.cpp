#include "InitDirect3DApp.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    InitDirect3DApp theApp(hInstance);

    if (!theApp.Init())
        return 0;

    return theApp.Run();
}
