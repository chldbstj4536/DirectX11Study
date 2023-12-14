#include "ExampleApp.h"

ExampleApp::ExampleApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
    , mBoxVB(nullptr)
    , mBoxIB(nullptr)
    , mFX(nullptr)
    , mTech(nullptr)
    , mfxWorldViewProj(nullptr)
    , mInputLayout(nullptr)
    , mTheta(1.5f * MathHelper::Pi)
    , mPhi(0.25f * MathHelper::Pi)
    , mRadius(5.0f)
{
    mMainWndCaption = L"Box Demo";

    mLastMousePos.x = 0;
    mLastMousePos.y = 0;

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mWorld, I);
    XMStoreFloat4x4(&mView, I);
    XMStoreFloat4x4(&mProj, I);
}

ExampleApp::~ExampleApp()
{
    ReleaseCOM(mBoxVB);
    ReleaseCOM(mBoxIB);
    ReleaseCOM(mFX);
    ReleaseCOM(mInputLayout);
}

bool ExampleApp::Init()
{
    if (!D3DApp::Init())
        return false;

    BuildGeometryBuffers();
    BuildFX();
    BuildVertexLayout();

    return true;
}

void ExampleApp::OnResize()
{
    D3DApp::OnResize();

    // 창의 크기가 변했으므로 종횡비를 갱신하고 투영 행렬을 다시 계산한다.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void ExampleApp::UpdateScene(float dt)
{
    // 구면 좌표를 데카르트 좌표로 변환한다.
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // 시야 행렬을 구축한다.
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, V);
}

void ExampleApp::DrawScene()
{
    assert(md3dImmediateContext);
    assert(mSwapChain);

    // 후면 버퍼를 파란색으로 지운다. Colors::Blue는 d3dUtil.h에 정의되어 있다.
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));

    // 깊이 버퍼를 1.0f, 스텐실 버퍼를 0으로 지운다.
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(mInputLayout);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
    md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

    // 상수들을 설정한다.
    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;

    mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

        // 색인 36개로 상자를 그린다.
        md3dImmediateContext->DrawIndexed(36, 0, 0);
    }

    // 후면 버퍼를 화면에 제시한다.
    HR(mSwapChain->Present(0, 0));
}

void ExampleApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}

void ExampleApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void ExampleApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        // 1픽셀이 4분의 1도 (degree 단위)가 되게 한다.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        // 마우스 입력에 기초한 각도로 상자 주변의 궤도 카메라를 갱신한다.
        mTheta += dx;
        mPhi += dy;

        // 각도를 mphi로 한정한다.
        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // 1픽셀이 장면의 0.005단위가 되게 한다.
        float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

        // 마우스 입력에 기초해서 궤도 카메라의 반지름을 갱신한다.
        mRadius += dx - dy;

        // 반지름을 특정 범위로 한정한다.
        mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void ExampleApp::BuildGeometryBuffers()
{
    // 정점 버퍼를 생성한다.
    Vertex vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f),    (const float*)&Colors::White },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f),     (const float*)&Colors::Black },
        { XMFLOAT3(1.0f, 1.0f, -1.0f),      (const float*)&Colors::Red },
        { XMFLOAT3(1.0f, -1.0f, -1.0f),     (const float*)&Colors::Green },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f),     (const float*)&Colors::Blue },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f),      (const float*)&Colors::Yellow },
        { XMFLOAT3(1.0f, 1.0f, 1.0f),       (const float*)&Colors::Cyan },
        { XMFLOAT3(1.0f, -1.0f, 1.0f),      (const float*)&Colors::Magenta },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

    // 색인 버퍼를 생성한다.
    UINT indices[] =
    {
        // 상자의 앞면
        0, 1, 2,
        0, 2, 3,

        // 뒷면
        4, 6, 5,
        4, 7, 6,

        // 왼쪽 면
        4, 5, 1,
        4, 1, 0,

        // 오른쪽 면
        3, 2, 6,
        3, 6, 7,

        // 윗면
        1, 5, 6,
        1, 6, 2,

        // 밑면
        4, 0, 3,
        4, 3, 7,
    };

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void ExampleApp::BuildFX()
{
    DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags != D3D10_SHADER_DEBUG;
    shaderFlags != D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    ID3D10Blob* compiledShader = 0;
    ID3D10Blob* compilationMsgs = 0;

    HRESULT hr = D3DX11CompileFromFile(L"D:/Project/VisualStudio/DirectX11Study/FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);

    // compilationMsgs에 오류 메시지나 경고 메시지가 저장되어 있을 수 있다.
    if (compilationMsgs != 0)
    {
        MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
        ReleaseCOM(compilationMsgs);
    }

    // compilationMsgs에 오류 메시지가 없었다고 해도 컴파일 오류 여부를
    // 명시적으로 점검하는 것이 바람직하다
    if (FAILED(hr))
    {
        assert(true);
    }

    HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, md3dDevice, &mFX));

    // 컴파일된 셰이더 자료를 다 사용했으므로 해제한다.
    ReleaseCOM(compiledShader);

    mTech = mFX->GetTechniqueByName("ColorTech");
    mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void ExampleApp::BuildVertexLayout()
{
    // 정점 입력 배치를 만든다.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    // 입력 배치 객체를 생성한다.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
}
