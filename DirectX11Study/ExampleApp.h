#pragma once

#include "d3dApp.h"

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 color;
};

class ExampleApp : public D3DApp
{
public:
    ExampleApp(HINSTANCE hInstance);
    virtual ~ExampleApp();

    virtual bool Init() override;
    virtual void OnResize() override;
    virtual void UpdateScene(float dt) override;
    virtual void DrawScene() override;

    virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
    void BuildGeometryBuffers();
    void BuildFX();
    void BuildVertexLayout();

private:
    ID3D11Buffer* mBoxVB;
    ID3D11Buffer* mBoxIB;

    ID3DX11Effect* mFX;
    ID3DX11EffectTechnique* mTech;
    ID3DX11EffectMatrixVariable* mfxWorldViewProj;

    ID3D11InputLayout* mInputLayout;

    XMFLOAT4X4 mWorld;
    XMFLOAT4X4 mView;
    XMFLOAT4X4 mProj;

    float mTheta;
    float mPhi;
    float mRadius;

    POINT mLastMousePos;
};
