#pragma once

#include "d3dApp.h"

class InitDirect3DApp : public D3DApp
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    virtual ~InitDirect3DApp() = default;

    virtual bool Init() override;
    virtual void OnResize() override;
    virtual void UpdateScene(float dt) override;
    virtual void DrawScene() override;
};
