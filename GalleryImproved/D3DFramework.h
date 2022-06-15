#pragma once
// Gallery template, based on the Microsoft DX11 tutorial 04
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <atlbase.h>
#include <memory>

#include "FileRead.h"
#include "GeneratePipeline.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "ModelManager.h"
#include "Dust.h"
#include "RenderTexture.h"


using namespace DirectX;

#define COMPILE_CSO


struct CBLight
{
	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
};
struct Light
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 pos;
	XMFLOAT3 dir;
	XMFLOAT3 att;
	float pad2;
	float cone;
	float range;
};

struct CBChangesEveryFrame
{
	XMMATRIX mView;
	XMMATRIX mWorld;
	XMFLOAT4 lightPos[4];
	XMFLOAT2 time;
	XMFLOAT3 eye;
	XMFLOAT2 model;
};


class D3DFramework {

	HINSTANCE _hInst = nullptr;
	HWND _hWnd = nullptr;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_1;
	CComPtr <ID3D11Device> _pd3dDevice;
	CComPtr <ID3D11Device1> _pd3dDevice1;
	CComPtr <ID3D11DeviceContext> _pImmediateContext;
	CComPtr <ID3D11DeviceContext1> _pImmediateContext1;
	CComPtr <IDXGISwapChain1> _swapChain;
	CComPtr <IDXGISwapChain1> _swapChain1;
	CComPtr <ID3D11RenderTargetView> _pRenderTargetView;
	D3D11_VIEWPORT vp_{}; 
	D3D11_VIEWPORT shadow_vp_{};
	HRESULT SetUp();
	HRESULT hr_{};
	D3D11_BUFFER_DESC bd_ = {};
	//Shaders
	UINT width_{};
	UINT height_{};
	bool dust_fall_ = true;


	CComPtr <ID3D11InputLayout> _pVertexLayout;

	ID3D11Buffer* g_pCBChangeOnResize = nullptr;
	ID3D11Buffer* g_pCBChangesEveryFrame = nullptr;
	ID3D11Buffer* g_pCBLight = nullptr;

	XMMATRIX world_ = {};
	XMMATRIX view_ = {};
	XMMATRIX projection_ = {};
	XMMATRIX model_ = { };

	XMMATRIX _lightView = {};
	XMMATRIX _lightProjection = {};
	float _rotation = 0.0f;

	static unique_ptr<D3DFramework> _instance;

	XMFLOAT4* light_pos_{};
	 XMFLOAT4 local_light_{};
	XMFLOAT4 light_pos1_{};
	XMMATRIX plinth_ = {};
	XMMATRIX room_ = {};

	GeneratePipeline* gen_{};
	Dust* dust_{};

	ShaderManager* m_shaderManager{};
	ModelManager* m_modelManager{};
	int VSchoose = 0;
	float TimeSet();

	Camera cam1;
	Camera cam2;
	Camera* curCam{};

	RenderTexture* textRend{};

	ID3D11ShaderResourceView* m_ShadowSRV{};
	ID3D11Texture2D* m_ShadowTexture{};
	ID3D11DepthStencilView* m_shadowDSV{};

	int light_choice = 0;
	FileRead* m_read_file{};
	int models{};
	float time_scale_ = 1000;
	float time_{};
	CBChangesEveryFrame cb6{  };
public:
	D3DFramework() = default;
	D3DFramework(D3DFramework&) = delete;
	D3DFramework(D3DFramework&&) = delete;
	D3DFramework& operator=(const D3DFramework&) = delete;
	D3DFramework& operator=(const D3DFramework&&) = delete;
	~D3DFramework();

	HRESULT CreateBuffers(D3D11_BUFFER_DESC* bd);
	HRESULT LightConstantBuffer()const;
	static D3DFramework& GetInstance() { return *_instance; }

	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** const ppBlobOut);
	HRESULT InitDevice();
	void Render();
	void RoomMake();
	void Shadow();
	void DustMake(float t) ;
	void CameraSet();
	void ManagersSetUp();
	void UpdateView();
	void PipelineSet1(const int shader) const;
	void PipelineSet2(const int shader);
	void MatrixIntialize();
};