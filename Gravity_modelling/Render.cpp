#include "Render.h"
Render *render = new Render();
Render::Render()
{
    driverType = D3D_DRIVER_TYPE_NULL;
    featureLevel = D3D_FEATURE_LEVEL_11_0;
    pd3dDevice = nullptr;
    pImmediateContext = nullptr;
    pSwapChain = nullptr;
    pRenderTargetView = nullptr;
	pDepthStencil = nullptr;
	pDepthStencilView = nullptr;
    pVertexShader = nullptr;
    pPixelShader = nullptr;
	pPixelShaderNoLight = nullptr;
    pVertexLayout = nullptr;
    pVertexBuffer = nullptr;
	pIndexBuffer = nullptr;        
    pConstantBuffer = nullptr;  
	pStarsTexture = nullptr;
	pSunTexture = nullptr;
	pEarthTexture = nullptr;
	pMoonTexture = nullptr;
    pSamplerLinear = nullptr; 
}
void Render::init(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc );
    UINT width = rc.right - rc.left;	
    UINT height = rc.bottom - rc.top;	
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );
	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );
    DXGI_SWAP_CHAIN_DESC sd;			
    ZeroMemory( &sd, sizeof( sd ) );	
	sd.BufferCount = 1;					
    sd.BufferDesc.Width = width;		
    sd.BufferDesc.Height = height;		
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
    sd.BufferDesc.RefreshRate.Numerator = 60;			
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
    sd.OutputWindow = hwnd;							
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.Windowed = TRUE;						
    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        driverType = driverTypes[driverTypeIndex];
        HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pImmediateContext );
        if (SUCCEEDED(hr))
			break;
	}
    ID3D11Texture2D* pBackBuffer = NULL;
    pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
    pBackBuffer->Release();
    D3D11_TEXTURE2D_DESC descDepth;	
	ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;		
    descDepth.Height = height;		
    descDepth.MipLevels = 1;		
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;		
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    pd3dDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;	
	ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;		
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    pd3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView );
    pImmediateContext->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pImmediateContext->RSSetViewports( 1, &vp );

}
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}
void Render::init_geometry(HWND hwnd)
{
    ID3DBlob* pVSBlob = NULL; 
    CompileShaderFromFile( L"Shaders.fx", "VS", "vs_4_0", &pVSBlob );
	pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVertexShader );
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );
    pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &pVertexLayout );
	pVSBlob->Release();
    pImmediateContext->IASetInputLayout(pVertexLayout );
	ID3DBlob* pPSBlob = NULL;
    CompileShaderFromFile( L"Shaders.fx", "PS", "ps_4_0", &pPSBlob );
	pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShader);
	pPSBlob->Release();
	pPSBlob = NULL;
	CompileShaderFromFile(L"Shaders.fx", "PS_no_light", "ps_4_0", &pPSBlob);
	pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShaderNoLight);
  	pPSBlob->Release();

	Assimp::Importer importer;
	const aiScene* spherescene = importer.ReadFile("sphere.obj", 
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_FindInvalidData
		);

	buffers = new Buffers[1];
	buffers[0].vertexcount = spherescene->mMeshes[0]->mNumVertices;
	buffers[0].indexcount = (spherescene->mMeshes[0]->mNumFaces * spherescene->mMeshes[0]->mFaces->mNumIndices) * 2;
	buffers[0].vertices = new Vertex[buffers[0].vertexcount];
	
	for (int i = 0; i < buffers[0].vertexcount; i++)
	{
		buffers[0].vertices[i].Pos = XMFLOAT3(spherescene->mMeshes[0]->mVertices[i].x, spherescene->mMeshes[0]->mVertices[i].y, spherescene->mMeshes[0]->mVertices[i].z);
		buffers[0].vertices[i].Tex = XMFLOAT2(spherescene->mMeshes[0]->mTextureCoords[0][i].x, spherescene->mMeshes[0]->mTextureCoords[0][i].y);
		buffers[0].vertices[i].Norm = XMFLOAT3(spherescene->mMeshes[0]->mNormals[i].x, spherescene->mMeshes[0]->mNormals[i].y, spherescene->mMeshes[0]->mNormals[i].z);
	}

	buffers[0].indices = new WORD[buffers[0].indexcount];
	int j = 0;
	for (int i = 0; i < spherescene->mMeshes[0]->mNumFaces; i++)
	{
		buffers[0].indices[j] = spherescene->mMeshes[0]->mFaces[i].mIndices[0];
		buffers[0].indices[j+1] = spherescene->mMeshes[0]->mFaces[i].mIndices[2];
		buffers[0].indices[j+2] = spherescene->mMeshes[0]->mFaces[i].mIndices[1];

		buffers[0].indices[j+3] = spherescene->mMeshes[0]->mFaces[i].mIndices[0];
		buffers[0].indices[j+4] = spherescene->mMeshes[0]->mFaces[i].mIndices[1];
		buffers[0].indices[j+5] = spherescene->mMeshes[0]->mFaces[i].mIndices[2];
		j += 6;
	}

//----------------------------------------------------------------------------------------------------------------------------------//
	D3D11_BUFFER_DESC bd;	
	ZeroMemory(&bd, sizeof(bd));
    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);	
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	
	bd.CPUAccessFlags = 0;
    pd3dDevice->CreateBuffer( &bd, NULL, &pConstantBuffer );
    D3DX11CreateShaderResourceViewFromFile( pd3dDevice, L"stars.dds", NULL, NULL, &pStarsTexture, NULL );
    D3DX11CreateShaderResourceViewFromFile( pd3dDevice, L"sun.dds", NULL, NULL, &pSunTexture, NULL );
	D3DX11CreateShaderResourceViewFromFile( pd3dDevice, L"Earth.dds", NULL, NULL, &pEarthTexture, NULL);
    D3DX11CreateShaderResourceViewFromFile( pd3dDevice, L"moon.dds", NULL, NULL, &pMoonTexture, NULL );  
	D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;         
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    pd3dDevice->CreateSamplerState( &sampDesc, &pSamplerLinear );
}
void Render::setbuffers(Buffers bufferz)
{
	if (pVertexBuffer)
		pVertexBuffer->Release();
	if (pIndexBuffer)
		pIndexBuffer->Release();
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * bufferz.vertexcount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = bufferz.vertices;
	pd3dDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * bufferz.indexcount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = bufferz.indices;
	pd3dDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

void Render::reinit(HWND hwnd, bool Fullscr)
{
	pRenderTargetView->Release();
	pDepthStencil->Release();
	pDepthStencilView->Release();
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	DXGI_MODE_DESC dc;
	dc.Format = DXGI_FORMAT_UNKNOWN;
	dc.Width = width;
	dc.Height = height;
	dc.RefreshRate.Numerator = 60;
	dc.RefreshRate.Denominator = 1;
	dc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	pSwapChain->ResizeTarget(&dc);
	ID3D11Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	pd3dDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	pd3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDepthStencilView);
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &vp);

	XMStoreFloat4x4(&Projection, XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 2000000.0f));
	if (Fullscr)
		pSwapChain->SetFullscreenState(TRUE, NULL);
	else
		pSwapChain->SetFullscreenState(FALSE, NULL);
}
void Render::Draw()
{
	static bool end = false;
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
	static float ScaleCoeff = 0.1f;
    pImmediateContext->ClearRenderTargetView( pRenderTargetView, ClearColor );
    pImmediateContext->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	setbuffers(buffers[0]);
	pImmediateContext->PSSetSamplers( 0, 1, &pSamplerLinear );
	pImmediateContext->VSSetShader( pVertexShader, NULL, 0 );
	pImmediateContext->VSSetConstantBuffers( 0, 1, &pConstantBuffer );
	pImmediateContext->PSSetShaderResources( 0, 1, &pStarsTexture );
	pImmediateContext->PSSetShader( pPixelShaderNoLight, NULL, 0 );
	SetStars();
	pImmediateContext->DrawIndexed( buffers[0].indexcount, 0, 0 );
	if (Gravity_center::g_c)
	if (!end)
	{
	    SetGravityCenter(ScaleCoeff);
		pImmediateContext->PSSetShaderResources( 0, 1, &pSunTexture );
		pImmediateContext->DrawIndexed(buffers[0].indexcount, 0, 0);
	}
	else
	{
		SetSuperNova(ScaleCoeff);
		pImmediateContext->PSSetShaderResources( 0, 1, &pSunTexture );
		pImmediateContext->DrawIndexed(buffers[0].indexcount, 0, 0 );
	}
	static float t = 0.0f;
    static DWORD TimeStart = 0;
	if (Gravity_center::gravity_on)
	{
        DWORD TimeCur = GetTickCount();
        if( TimeStart == 0)
            TimeStart = TimeCur;
	    t = ( TimeCur - TimeStart ) / 1000.0f;
	}
	pImmediateContext->PSSetShaderResources( 0, 1, &pMoonTexture );
	pImmediateContext->PSSetShader(pPixelShader, NULL, 0);
	if (!physic_object::objects.empty())
			for (int i = 0; i<physic_object::objects.size(); i++)
			{ 
				if (Gravity_center::gravity_on)
				{
	                physic_object::objects.at(i)->pos.x =  ((physic_object::objects.at(i)->get_x()+(Gravity_center::g_c->get_x()-physic_object::objects.at(i)->get_x())*(physic_object::objects.at(i)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(i)->pos.dx;  // here i used parametric equation of the line (x = x0+ax*lambda) where x0 = start
				    physic_object::objects.at(i)->pos.y =  ((physic_object::objects.at(i)->get_y()+(Gravity_center::g_c->get_y()-physic_object::objects.at(i)->get_y())*(physic_object::objects.at(i)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(i)->pos.dy;  // point of object, ax - x coordinate of the direction vector, which is calculated by formula Xend-Xstart;
				    physic_object::objects.at(i)->pos.z =  ((physic_object::objects.at(i)->get_z()+(Gravity_center::g_c->get_z()-physic_object::objects.at(i)->get_z())*(physic_object::objects.at(i)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(i)->pos.dz;  // lambda = parameter - (a*t^2)/2	
				    if ( (physic_object::objects.at(i)->pos.x >= -3.0f && physic_object::objects.at(i)->pos.x <=  3.0f) && (physic_object::objects.at(i)->pos.y >= -3.0f && physic_object::objects.at(i)->pos.y <=  3.0f) && (physic_object::objects.at(i)->pos.z >= - 3.0f &&  physic_object::objects.at(i)->pos.z <=  3.0f) )
				    {
						Gravity_center::g_c->set_m(physic_object::objects.at(i)->get_m());
						for (int j=0; j<physic_object::objects.size(); j++)
						{
							physic_object::objects.at(j)->pos.dx = ((physic_object::objects.at(j)->get_x()+(Gravity_center::g_c->get_x()-physic_object::objects.at(j)->get_x())*(physic_object::objects.at(j)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(j)->pos.x;
						    physic_object::objects.at(j)->pos.dy = ((physic_object::objects.at(j)->get_y()+(Gravity_center::g_c->get_y()-physic_object::objects.at(j)->get_y())*(physic_object::objects.at(j)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(j)->pos.y;
						    physic_object::objects.at(j)->pos.dz = ((physic_object::objects.at(j)->get_z()+(Gravity_center::g_c->get_z()-physic_object::objects.at(j)->get_z())*(physic_object::objects.at(j)->a(*Gravity_center::g_c)*pow(t,2)/2)))-physic_object::objects.at(j)->pos.z;
						}
				     	physic_object::objects.erase(physic_object::objects.begin()+i);
					    ScaleCoeff+=0.007f;
					    continue;
				    }
				    SetSphere(physic_object::objects.at(i)->get_x()+(physic_object::objects.at(i)->pos.x-physic_object::objects.at(i)->get_x()),physic_object::objects.at(i)->get_y()+(physic_object::objects.at(i)->pos.y-physic_object::objects.at(i)->get_y()),physic_object::objects.at(i)->get_z()+(physic_object::objects.at(i)->pos.z-physic_object::objects.at(i)->get_z()));
				}
				else
                    SetSphere(physic_object::objects.at(i)->get_x(),physic_object::objects.at(i)->get_y(),physic_object::objects.at(i)->get_z());
			    pImmediateContext->DrawIndexed(buffers[0].indexcount, 0, 0 );
			}
		else
			if (Gravity_center::gravity_on)
			    end = true;
    pSwapChain->Present( 0, 0 );
}
void Render::InitMatrixes(HWND hwnd)
{
  RECT rc;
    GetClientRect( hwnd, &rc );
    UINT width = rc.right - rc.left;	
    UINT height = rc.bottom - rc.top;	

	XMStoreFloat4x4(&World,XMMatrixIdentity());

	XMVECTOR Eye = XMVectorSet( 15.0f, 0.0f, 0.0f, 0.0f );	
	XMVECTOR At = XMVectorSet( 14.0f, 0.0f, 0.0f, 0.0f );	
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );	
	XMStoreFloat4x4(&View,XMMatrixLookAtLH( Eye, At, Up ));
	XMStoreFloat4x4(&Projection,XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 10000000.0f ));
}
void Render::SetGravityCenter(float Coeff)
{
    static float t = 0.0f;
    static DWORD TimeStart = 0;
    DWORD TimeCur = GetTickCount();
    if( TimeStart == 0)
        TimeStart = TimeCur;
	t = ( TimeCur - TimeStart ) / 1000.0f;
	XMMATRIX mSpin = XMMatrixRotationY(t);
	XMMATRIX mScale = XMMatrixScaling(Coeff-0.05f,Coeff-0.05f,Coeff-0.05f);
	XMStoreFloat4x4(&World,mScale*mSpin);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose( XMLoadFloat4x4(&World) );
	cb.mView = XMMatrixTranspose( XMLoadFloat4x4(&View) );
	cb.mProjection = XMMatrixTranspose( XMLoadFloat4x4(&Projection) );
	pImmediateContext->UpdateSubresource( pConstantBuffer, 0, NULL, &cb, 0, 0 );
}
void Render::SetSuperNova(float Coeff)
{
	Coeff-=0.05f;
    static float t = 0.0f;
    static DWORD TimeStart = 0;
    DWORD TimeCur = GetTickCount();
    if( TimeStart == 0)
        TimeStart = TimeCur;
	t = ( TimeCur - TimeStart ) / 1000.0f;
	XMMATRIX mScale;
	XMMATRIX mSpin = XMMatrixRotationY(t);
	mScale = XMMatrixScaling(t*(t/10)+Coeff,t*(t/10)+Coeff,t*(t/10)+Coeff);
	XMStoreFloat4x4(&World,mScale*mSpin);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose( XMLoadFloat4x4(&World) );
	cb.mView = XMMatrixTranspose( XMLoadFloat4x4(&View) );
	cb.mProjection = XMMatrixTranspose( XMLoadFloat4x4(&Projection) );
	pImmediateContext->UpdateSubresource( pConstantBuffer, 0, NULL, &cb, 0, 0 );
}

void Render::SetStars()
{
	XMMATRIX mScale = XMMatrixScaling(2000,2000,2000);
	XMStoreFloat4x4(&World,mScale);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose( XMLoadFloat4x4(&World) );
	cb.mView = XMMatrixTranspose( XMLoadFloat4x4(&View) );
	cb.mProjection = XMMatrixTranspose( XMLoadFloat4x4(&Projection) );
	pImmediateContext->UpdateSubresource( pConstantBuffer, 0, NULL, &cb, 0, 0 );
}

void Render::SetSphere(float dx, float dy, float dz)
{
	XMMATRIX mTranslate = XMMatrixTranslation(dx,dy,dz);
	XMMATRIX mScale = XMMatrixScaling(0.01f,0.01f,0.01f);
	XMStoreFloat4x4(&World,mTranslate*mScale);
	ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose( XMLoadFloat4x4(&World) );
	cb.mView = XMMatrixTranspose( XMLoadFloat4x4(&View) );
	cb.mProjection = XMMatrixTranspose( XMLoadFloat4x4(&Projection) );
	pImmediateContext->UpdateSubresource( pConstantBuffer, 0, NULL, &cb, 0, 0 );
}
void Render::SetViewMatrix(FLOAT x, FLOAT y, FLOAT z, FLOAT vx, FLOAT vy, FLOAT vz)
{
	XMVECTOR Eye = XMVectorSet( x, y, z, 0.0f );	
	XMVECTOR At = XMVectorSet( vx, vy, vz, 0.0f );	
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );	
	XMStoreFloat4x4(&View,XMMatrixLookAtLH( Eye, At, Up ));
}
Render::~Render()
{
	pSwapChain->SetFullscreenState(false, NULL);
	pd3dDevice->Release();
	pImmediateContext->ClearState();
    pImmediateContext->Release();
    pSwapChain->Release();
    pRenderTargetView->Release();
	pDepthStencil->Release();
    pDepthStencilView->Release();
	pVertexShader->Release();
    pPixelShader->Release();
	pPixelShaderNoLight->Release();
    pVertexLayout->Release();
    pVertexBuffer->Release();
	pIndexBuffer->Release();        
    pConstantBuffer->Release();  
	pStarsTexture->Release();
	pSunTexture->Release();
	pEarthTexture->Release();
	pMoonTexture->Release();
    pSamplerLinear->Release(); 
}