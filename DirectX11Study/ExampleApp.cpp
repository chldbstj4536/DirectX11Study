#include "ExampleApp.h"
#include <GeometryGenerator.h>

ExampleApp::ExampleApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
    , mVB(nullptr)
    , mIB(nullptr)
    , mFX(nullptr)
    , mTech(nullptr)
    , mfxWorld(nullptr)
    , mfxWorldInvTranspose(nullptr)
    , mfxWorldViewProj(nullptr)
    , mfxDirLight(nullptr)
    , mfxPointLight(nullptr)
    , mfxSpotLight(nullptr)
    , mfxEyePosW(nullptr)
    , mfxMaterial(nullptr)
    , mInputLayout(nullptr)
    , mTheta(1.5f * MathHelper::Pi)
    , mPhi(0.25f * MathHelper::Pi)
    , mRadius(20.0f)
{
    mMainWndCaption = L"Shapes Demo";

    mLastMousePos.x = 0;
    mLastMousePos.y = 0;

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mView, I);
    XMStoreFloat4x4(&mProj, I);

    // ���Ɽ�� ����
    mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    mDirLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

    // ������ ���� -- ��ġ�� �ִϸ��̼��� ���� �� �����Ӹ���
    // UpdateScene �޼��忡�� ���ŵȴ�.
    mPointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    mPointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
    mPointLight.Range = 25.0f;

    // �������� ���� -- ��ġ�� ������ �ִϸ��̼��� ���� �� �����Ӹ���
    // UpdateScene �޼��忡�� ���ŵȴ�.
    mSpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mSpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mSpotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
    mSpotLight.Spot = 96.0f;
    mSpotLight.Range = 10000.0f;

    mLandMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mLandMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

    mCylinderMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    mCylinderMat.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    mCylinderMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
}

ExampleApp::~ExampleApp()
{
    ReleaseCOM(mVB);
    ReleaseCOM(mIB);
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

    // ���� �������� ���� ���������� ��ȯ ����� ����
    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mGridWorld, I);

    XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
    XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

    XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
    XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

    // ����յ��� �ټ��ٷ� ��ġ�Ǵµ�, �ϳ��� ���� �� ���� ����� + �� ������ �̷������.
    for (int i = 0; i < 5; ++i)
    {
        XMStoreFloat4x4(&mCylinderWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
        XMStoreFloat4x4(&mCylinderWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

        XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
        XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
    }
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

    mEyePosW = XMFLOAT3(x, y, z);

    // �þ� ����� �����Ѵ�.
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, V);

    // ������ ���� ������ ���� �׸��鼭 �̵��ϰ� �Ѵ�.
    mPointLight.Position.x = 70.0f * cosf(0.2f * mTimer.TotalTime());
    mPointLight.Position.z = 70.0f * sinf(0.2f * mTimer.TotalTime());
    mPointLight.Position.y = 10.0f;

    // ���������� ī�޶��� ��ġ���� ī�޶� �ٶ󺸴� �������� ���� ������.
    // �̿� ����, ��ġ �����ڰ� �������� ��� �ٴϴ� ���� ȿ���� ����.
    mSpotLight.Position = mEyePosW;
    XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));
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
    md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

    // ������� �����Ѵ�.
    XMMATRIX world;
    XMMATRIX worldInvTranspose;
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX viewProj = view * proj;
    XMMATRIX worldViewProj;

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc(&techDesc);

    mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
    mfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));
    mfxSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));
    mfxEyePosW->SetRawValue(&mEyePosW, 0, sizeof(mEyePosW));


    // �׸��� �׸���
    world = XMLoadFloat4x4(&mGridWorld);
    worldInvTranspose = MathHelper::InverseTranspose(world);
    worldViewProj = world * viewProj;
    mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
    mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
    mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
    mfxMaterial->SetRawValue(&mLandMat, 0, sizeof(mLandMat));
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);
    }


    mfxMaterial->SetRawValue(&mLandMat, 0, sizeof(mLandMat));

    // �� �׸���
    for (int i = 0; i < 10; ++i)
    {
        world = XMLoadFloat4x4(&mSphereWorld[i]);
        worldInvTranspose = MathHelper::InverseTranspose(world);
        worldViewProj = world * viewProj;
        mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
        mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
        mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
            md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
        }
    }

    // ���� �׸���
    for (int i = 0; i < 10; ++i)
    {
        world = XMLoadFloat4x4(&mCylinderWorld[i]);
        worldInvTranspose = MathHelper::InverseTranspose(world);
        worldViewProj = world * viewProj;
        mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
        mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
        mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
            md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
        }
    }


    // ���� �׸���
    world = XMLoadFloat4x4(&mBoxWorld);
    worldInvTranspose = MathHelper::InverseTranspose(world);
    worldViewProj = world * viewProj;
    mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
    mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
    mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
    }


    // �߾� ��ü �׸���
    worldViewProj = XMLoadFloat4x4(&mCenterSphere) * viewProj;
    mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
        md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
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
    GeometryGenerator::MeshData grid;
    GeometryGenerator::MeshData sphere;
    GeometryGenerator::MeshData cylinder;
    GeometryGenerator::MeshData box;

    GeometryGenerator geoGen;
    geoGen.CreateGrid(20.0f, 20.0f, 60, 40, grid);
    geoGen.CreateSphere(0.5f, 20, 20, sphere);
    geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);
    geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

    mGridVertexOffset = 0;
    mSphereVertexOffset = grid.Vertices.size();
    mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();
    mBoxVertexOffset = mCylinderVertexOffset + cylinder.Vertices.size();

    mGridIndexCount = grid.Indices.size();
    mSphereIndexCount = sphere.Indices.size();
    mCylinderIndexCount = cylinder.Indices.size();
    mBoxIndexCount = box.Indices.size();

    mGridIndexOffset = 0;
    mSphereIndexOffset = grid.Indices.size();
    mCylinderIndexOffset = mSphereIndexOffset + sphere.Indices.size();
    mBoxIndexOffset = mCylinderIndexOffset + cylinder.Indices.size();

    UINT totalVertexCount = grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size() + box.Vertices.size();

    std::vector<Vertex> vertices(totalVertexCount);
    UINT i = 0;
    for (const auto& v : grid.Vertices)
    {
        vertices[i].pos = v.Position;
        vertices[i].normal = v.Normal;
        ++i;
    }
    for (const auto& v : sphere.Vertices)
    {
        vertices[i].pos = v.Position;
        vertices[i].normal = v.Normal;
        ++i;
    }
    for (const auto& v : cylinder.Vertices)
    {
        vertices[i].pos = v.Position;
        vertices[i].normal = v.Normal;
        ++i;
    }
    for (const auto& v : box.Vertices)
    {
        vertices[i].pos = v.Position;
        vertices[i].normal = v.Normal;
        ++i;
    }

    std::vector<UINT> indices;
    indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
    indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
    indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
    indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices.data();

    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices.data();

    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
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
#ifdef _UNICODE
    std::wstring path = TEXT(_OUTPUT_PATH);
#else
    std::string path = TEXT(_OUTPUT_PATH);
#endif
    path.append(TEXT("FX/color.fx"));

    HRESULT hr = D3DX11CompileFromFile(path.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);

    // compilationMsgs�� ���� �޽����� ��� �޽����� ����Ǿ� ���� �� �ִ�.
    if (compilationMsgs != 0)
    {
        MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
        ReleaseCOM(compilationMsgs);
    }

    // compilationMsgs�� ���� �޽����� �����ٰ� �ص� ������ ���� ���θ�
    // ���������� �����ϴ� ���� �ٶ����ϴ�
    if (FAILED(hr))
    {
        assert(true);
    }

    HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, md3dDevice, &mFX));

    // �����ϵ� ���̴� �ڷḦ �� ��������Ƿ� �����Ѵ�.
    ReleaseCOM(compiledShader);

    mTech = mFX->GetTechniqueByName("ColorTech");
    mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
    mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
    mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
    mfxDirLight = mFX->GetVariableByName("gDirLight");
    mfxPointLight = mFX->GetVariableByName("gPointLight");
    mfxSpotLight = mFX->GetVariableByName("gSpotLight");
    mfxEyePosW = mFX->GetVariableByName("gEyePosW");
    mfxMaterial = mFX->GetVariableByName("gMaterial");
}

void ExampleApp::BuildVertexLayout()
{
    // ���� �Է� ��ġ�� �����.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    // �Է� ��ġ ��ü�� �����Ѵ�.
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, sizeof(vertexDesc) / sizeof(vertexDesc[0]), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));
}

