#pragma once

#include "d3dApp.h"

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT3 normal;
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
    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

    ID3DX11Effect* mFX;
    ID3DX11EffectTechnique* mTech;
    ID3DX11EffectMatrixVariable* mfxWorld;
    ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
    ID3DX11EffectMatrixVariable* mfxWorldViewProj;
    ID3DX11EffectVariable* mfxDirLight;
    ID3DX11EffectVariable* mfxPointLight;
    ID3DX11EffectVariable* mfxSpotLight;
    ID3DX11EffectVariable* mfxEyePosW;
    ID3DX11EffectVariable* mfxMaterial;

    ID3D11InputLayout* mInputLayout;

    XMFLOAT4X4 mGridWorld;
    XMFLOAT4X4 mSphereWorld[10];
    XMFLOAT4X4 mCylinderWorld[10];
    XMFLOAT4X4 mBoxWorld;
    XMFLOAT4X4 mCenterSphere;
    XMFLOAT4X4 mView;
    XMFLOAT4X4 mProj;

    XMFLOAT3 mEyePosW;

    DirectionalLight mDirLight;
    PointLight mPointLight;
    SpotLight mSpotLight;
    Material mLandMat;
    Material mCylinderMat;

    float mTheta;
    float mPhi;
    float mRadius;

    UINT mGridVertexOffset;
    UINT mGridIndexCount;
    UINT mGridIndexOffset;
    UINT mSphereVertexOffset;
    UINT mSphereIndexCount;
    UINT mSphereIndexOffset;
    UINT mCylinderVertexOffset;
    UINT mCylinderIndexCount;
    UINT mCylinderIndexOffset;
    UINT mBoxVertexOffset;
    UINT mBoxIndexCount;
    UINT mBoxIndexOffset;

    POINT mLastMousePos;
};
