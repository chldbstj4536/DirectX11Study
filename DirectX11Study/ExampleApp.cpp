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

    // â�� ũ�Ⱑ �������Ƿ� ��Ⱦ�� �����ϰ� ���� ����� �ٽ� ����Ѵ�.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void ExampleApp::UpdateScene(float dt)
{
    // ���� ��ǥ�� ��ī��Ʈ ��ǥ�� ��ȯ�Ѵ�.
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // �þ� ����� �����Ѵ�.
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

    // �ĸ� ���۸� �Ķ������� �����. Colors::Blue�� d3dUtil.h�� ���ǵǾ� �ִ�.
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));

    // ���� ���۸� 1.0f, ���ٽ� ���۸� 0���� �����.
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    md3dImmediateContext->IASetInputLayout(mInputLayout);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
    md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

    // ������� �����Ѵ�.
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

        // ���� 36���� ���ڸ� �׸���.
        md3dImmediateContext->DrawIndexed(36, 0, 0);
    }

    // �ĸ� ���۸� ȭ�鿡 �����Ѵ�.
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
        // 1�ȼ��� 4���� 1�� (degree ����)�� �ǰ� �Ѵ�.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        // ���콺 �Է¿� ������ ������ ���� �ֺ��� �˵� ī�޶� �����Ѵ�.
        mTheta += dx;
        mPhi += dy;

        // ������ mphi�� �����Ѵ�.
        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // 1�ȼ��� ����� 0.005������ �ǰ� �Ѵ�.
        float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

        // ���콺 �Է¿� �����ؼ� �˵� ī�޶��� �������� �����Ѵ�.
        mRadius += dx - dy;

        // �������� Ư�� ������ �����Ѵ�.
        mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void ExampleApp::BuildGeometryBuffers()
{
    // ���� ���۸� �����Ѵ�.
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

    // ���� ���۸� �����Ѵ�.
    UINT indices[] =
    {
        // ������ �ո�
        0, 1, 2,
        0, 2, 3,

        // �޸�
        4, 6, 5,
        4, 7, 6,

        // ���� ��
        4, 5, 1,
        4, 1, 0,

        // ������ ��
        3, 2, 6,
        3, 6, 7,

        // ����
        1, 5, 6,
        1, 6, 2,

        // �ظ�
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

    // compilationMsgs�� ���� �޽����� ��� �޽����� ����Ǿ� ���� �� �ִ�.
    if (compilationMsgs != 0)
    {
        MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
        ReleaseCOM(compilationMsgs);
    }

    // compilationMsgs�� ���� �޽����� �����ٰ� �ص� ������ ���� ���θ�
    // ��������� �����ϴ� ���� �ٶ����ϴ�
    if (FAILED(hr))
    {
        assert(true);
    }

    HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, md3dDevice, &mFX));

    // �����ϵ� ���̴� �ڷḦ �� ��������Ƿ� �����Ѵ�.
    ReleaseCOM(compiledShader);

    mTech = mFX->GetTechniqueByName("ColorTech");
    mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void ExampleApp::BuildVertexLayout()
{
    // ���� �Է� ��ġ�� �����.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    // �Է� ��ġ ��ü�� �����Ѵ�.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
}
