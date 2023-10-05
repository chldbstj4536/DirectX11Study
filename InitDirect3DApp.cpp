#include "InitDirect3DApp.h"

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
{
}

bool InitDirect3DApp::Init()
{
    if (!D3DApp::Init())
        return false;

    return true;
}

void InitDirect3DApp::OnResize()
{
    D3DApp::OnResize();
}

void InitDirect3DApp::UpdateScene(float dt)
{

}

void InitDirect3DApp::DrawScene()
{
    assert(md3dImmediateContext);
    assert(mSwapChain);

    // 후면 버퍼를 파란색으로 지운다. Colors::Blue는 d3dUtil.h에 정의되어 있다.
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));

    // 깊이 버퍼를 1.0f, 스텐실 버퍼를 0으로 지운다.
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 후면 버퍼를 화면에 제시한다.
    HR(mSwapChain->Present(0, 0));
}
