#pragma once
#include "includes.h"
#include "Physic_object.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Norm;
};

struct Buffers
{
	Vertex* vertices;
	WORD* indices;
	int vertexcount;
	int indexcount;
};

class Render
{
private:
	//Direct3D resources:
    D3D_DRIVER_TYPE         driverType;
    D3D_FEATURE_LEVEL       featureLevel;
    ID3D11Device*           pd3dDevice;
    ID3D11DeviceContext*    pImmediateContext;
    IDXGISwapChain*         pSwapChain;
    ID3D11RenderTargetView* pRenderTargetView;
	ID3D11Texture2D*        pDepthStencil;            
    ID3D11DepthStencilView* pDepthStencilView;          
	ID3D11VertexShader*     pVertexShader;
    ID3D11PixelShader*      pPixelShader;
	ID3D11PixelShader*      pPixelShaderNoLight;
    ID3D11InputLayout*      pVertexLayout;
	Buffers*                buffers;
    ID3D11Buffer*           pVertexBuffer;
	ID3D11Buffer*           pIndexBuffer;        
    ID3D11Buffer*           pConstantBuffer;          
	
	ID3D11ShaderResourceView* pStarsTexture;
	ID3D11ShaderResourceView* pSunTexture;
	ID3D11ShaderResourceView* pEarthTexture;
	ID3D11ShaderResourceView* pMoonTexture;
    ID3D11SamplerState*       pSamplerLinear; 

    XMFLOAT4X4              World;
    XMFLOAT4X4              View;
    XMFLOAT4X4              Projection;



public:
	Render();
	void init(HWND);
	void reinit(HWND, bool);
	void Draw();
	void init_geometry(HWND);
	void setbuffers(Buffers);
	void InitMatrixes(HWND);
	void SetGravityCenter(float);
	void SetSuperNova(float);
	void SetSphere(float, float, float);
	void SetViewMatrix(FLOAT,FLOAT,FLOAT,FLOAT,FLOAT,FLOAT);
	void SetStars();     
	~Render();
}extern *render;

struct ConstantBuffer
{
    XMMATRIX mWorld;            
    XMMATRIX mView;        
    XMMATRIX mProjection;   
};

