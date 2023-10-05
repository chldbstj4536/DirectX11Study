#include "InitDirect3DApp.h"

#include <vector>
#include <sstream>

using namespace std;

std::vector<IDXGIAdapter*> CheckEnumAdapters(HANDLE hc, IDXGIFactory* pFactory)
{
    UINT i = 0;
    IDXGIAdapter* pAdapter;
    std::vector<IDXGIAdapter*> vAdapters;
    while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++i;
    }

    wstringstream wss;
    wss << L"*** NUM ADAPTERS = " << i << L'\n';
    WriteConsole(hc, wss.str().c_str(), wss.str().size(), NULL, NULL);

    return vAdapters;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    AllocConsole();
    HANDLE hc = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    IDXGIFactory* pFactory;
    CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory));

    CheckEnumAdapters(hc, pFactory);

    InitDirect3DApp theApp(hInstance);

    if (!theApp.Init())
        return 0;

    return theApp.Run();
}
