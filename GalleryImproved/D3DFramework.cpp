#include <assimp/Importer.hpp>
#include <assimp/scene.h>         
#include <assimp/postprocess.h>
#include "D3DFramework.h"
#include <directxcolors.h>
#include <iostream>
#include <vector>
#include "FileRead.h"
#include "DDSTextureLoader.h"
#include "RenderTexture.h"
#include "Resource.h"
#include "ShaderManager.h"

std::unique_ptr<D3DFramework> D3DFramework::_instance = std::make_unique<D3DFramework>();

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK D3DFramework::wndProc(const HWND hWnd, UINT const message, const WPARAM wParam, const LPARAM lParam) {
	PAINTSTRUCT ps;
	std::string msg;
	const auto& app = D3DFramework::GetInstance();

	switch (message) {
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			PostQuitMessage(0);
		}
		if (GetAsyncKeyState(VK_F1))
		{
			_instance->curCam = &_instance->cam1;
		}
		if (GetAsyncKeyState(VK_F2))
		{
			_instance->curCam = &_instance->cam2;
		}
		if (GetAsyncKeyState(VK_F3))
		{
			_instance->models++;
			if (_instance->models > 1)
			{
				_instance->models = 0;
			}
		}
		if (GetAsyncKeyState(VK_F4))
		{
			_instance->VSchoose++;
			if (_instance->VSchoose > 5)
			{
				_instance->VSchoose = 0;
			}
		}
		if (GetAsyncKeyState(VK_F5))
		{
			_instance->light_choice++;
			if (_instance->light_choice > 2)_instance->light_choice = 0;
		}
		if (GetAsyncKeyState(VK_F6))
		{

		}
		if (GetAsyncKeyState(VK_F8))
		{
			_instance->dust_fall_ = !_instance->dust_fall_;
		}
		//Camera Controls
					//Camera Rotate left
		if ((GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A') || GetAsyncKeyState('J')) && !GetAsyncKeyState(VK_CONTROL))
		{
			_instance->curCam->AddYaw(-0.2);
		}

		//Camera Rotate Right
		if ((GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D') || GetAsyncKeyState('L')) && !GetAsyncKeyState(VK_CONTROL))
		{
			_instance->curCam->AddYaw(0.2);
		}
		//Camera Rotate Up
		if ((GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W') || GetAsyncKeyState('I')) && !GetAsyncKeyState(VK_CONTROL))
		{
			_instance->curCam->AddPitch(-0.2);
		}
		//Camera Rotate Down
		if ((GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S') || GetAsyncKeyState('K')) && !GetAsyncKeyState(VK_CONTROL))
		{
			_instance->curCam->AddPitch(0.2);
		}

		//PAN directional controls
					//Pan Left
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LEFT))
		{
			_instance->curCam->AdjustPosition(0.5, 0, 0);
		}
		//Pan Right
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_RIGHT))
		{
			_instance->curCam->AdjustPosition(-0.5, 0, 0);
		}
		//Pan forward
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_UP))
		{
			_instance->curCam->AdjustPosition(0, 0, 0.5);
		}
		//Pan Backward
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_DOWN))
		{
			_instance->curCam->AdjustPosition(0, 0, -0.5);
		}
		//Pan Down
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_NEXT))
		{
			_instance->curCam->AdjustPosition(0, 0.5, 0);
		}
		//Pan Up
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_PRIOR))
		{
			_instance->curCam->AdjustPosition(0, -0.5, 0);
		}
		if (GetAsyncKeyState('T'))
		{
			if (GetKeyState(VK_CAPITAL) || GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
			{
				_instance->time_scale_ -= 100;
			}
			else
			{
				_instance->time_scale_ += 100;
			}
		}

		if (GetAsyncKeyState('R'))
		{
			_instance->cam1.SetPosition(_instance->m_read_file->GetCamera1()[0]);
			_instance->cam2.SetPosition(_instance->m_read_file->GetCamera2()[0]);
		}

		break;

	case WM_KEYUP:
		switch (wParam) {
		case 'A':
			msg = "A released";
			break;
		case '1':
			msg = "1 released";
			break;
		default:
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::InitWindow(const HINSTANCE hInstance, const int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, reinterpret_cast<LPCTSTR>(IDI_GALLERY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Sohail Turner";
	wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_GALLERY));
	if (!RegisterClassEx(&wcex))
		return static_cast<HRESULT>(_HRESULT_TYPEDEF_(0x80004005L));

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"Sohail Turner", L"Direct3D 11 Gallery",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return _HRESULT_TYPEDEF_(0x80004005L);

	ShowWindow(_hWnd, nCmdShow);

	return static_cast<HRESULT>(0L);
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::CompileShaderFromFile(const WCHAR* const szFileName, const LPCSTR szEntryPoint, const LPCSTR szShaderModel, ID3DBlob** const ppBlobOut)
{
	auto dwShaderFlags = static_cast<DWORD>(D3DCOMPILE_ENABLE_STRICTNESS);
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	CComPtr <ID3DBlob> pErrorBlob;
	const auto hr1 = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr1)) {
		if (pErrorBlob)
			OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
		return hr1;
	}

	return static_cast<HRESULT>(0L);
}

HRESULT D3DFramework::SetUp()
{
	hr_ = static_cast<HRESULT>(0L);

	RECT rc;

	GetClientRect(_hWnd, &rc);
	width_ = rc.right - rc.left;
	height_ = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	auto numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	auto numFeatureLevels = static_cast<UINT>(ARRAYSIZE(featureLevels));

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		_driverType = driverTypes[driverTypeIndex];
		hr_ = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);

		if (hr_ == _HRESULT_TYPEDEF_(0x80070057L))
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr_ = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);

		if (SUCCEEDED(hr_))
			break;
	}
	if (FAILED(hr_))
		return hr_;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	CComPtr <IDXGIFactory1> dxgiFactory;
	{
		CComPtr <IDXGIDevice> dxgiDevice;
		hr_ = _pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr_)) {
			CComPtr <IDXGIAdapter> adapter;
			hr_ = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr_)) {
				hr_ = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
			}
		}
	}
	if (FAILED(hr_))
		return hr_;

	// Create swap chain
	CComPtr <IDXGIFactory2> dxgiFactory2;
	hr_ = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

	// DirectX 11.1 or later
	hr_ = _pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&_pd3dDevice1));
	if (SUCCEEDED(hr_)) {
		static_cast<void>(_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&_pImmediateContext1)));
	}
	if (FAILED(hr_))
		return hr_;

	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.Width = width_;
	sd.Height = height_;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R16G16B16A16_FLOAT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	hr_ = dxgiFactory2->CreateSwapChainForHwnd(_pd3dDevice, _hWnd, &sd, nullptr, nullptr, &_swapChain1);
	if (SUCCEEDED(hr_)) {
		hr_ = _swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&_swapChain));
	}
	if (FAILED(hr_))
		return hr_;
	// Note this tutorial doesn't handle full-screen swap chains so we block the ALT+ENTER shortcut
	//dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);

	if (FAILED(hr_))
		return hr_;

	// Create a render target view
	CComPtr <ID3D11Texture2D> pBackBuffer;
	hr_ = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr_))
		return hr_;

	hr_ = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	if (FAILED(hr_))
		return hr_;

	//_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, g_pDepthStencilView);

	// Setup the viewport
	vp_.Width = static_cast<FLOAT>(width_);
	vp_.Height = static_cast<FLOAT>(height_);
	vp_.MinDepth = 0.0f;
	vp_.MaxDepth = 1.0f;
	vp_.TopLeftX = 0;
	vp_.TopLeftY = 0;

	/*shadowVP.Width = 2048;
	shadowVP.Height = 2048;
	shadowVP.MinDepth = 0.0f;
	shadowVP.MaxDepth = 1.0f;
	shadowVP.TopLeftX = 0;
	shadowVP.TopLeftY = 0;*/
	return hr_;
}
//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::InitDevice()
{
	hr_ = static_cast<HRESULT>(0L);

	hr_ = SetUp();

	if (FAILED(hr_))
		return hr_;
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	auto numElements = static_cast<UINT>(ARRAYSIZE(layout));

#ifdef COMPILE_CSO

	// Compile the vertex shader
	CComPtr <ID3DBlob> pVSBlob;
	hr_ = CompileShaderFromFile(L"Gallery.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr_)) {
		MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr_;
	}



	// Create the input layout
	hr_ = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout);
	if (FAILED(hr_))
		return hr_;
#else
	{
		const std::string fileName{ "..\\Debug\\Gallery_VS.cso" };
		std::ifstream fin(fileName, std::ios::binary);
		if (!fin) {
			MessageBox(nullptr, L"The CSO file cannot be found.", L"Error", MB_OK);
			return E_FAIL;
		}
		std::vector<unsigned char> byteCode(std::istreambuf_iterator<char>(fin), {});

		hr = _pd3dDevice->CreateVertexShader(&byteCode[0], byteCode.size(), nullptr, &_phongVS);
		if (FAILED(hr)) {
			return hr;
		}

		// Create the input layout
		hr = _pd3dDevice->CreateInputLayout(layout, numElements, &byteCode[0], byteCode.size(), &_pVertexLayout);
		if (FAILED(hr))
			return hr;
	}
#endif
	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);
#ifdef COMPILE_CSO
#else
	{
		const std::string fileName{ "..\\Debug\\Gallery_PS.cso" };
		std::ifstream fin(fileName, std::ios::binary);
		if (!fin) {
			MessageBox(nullptr, L"The CSO file cannot be found.", L"Error", MB_OK);
			return E_FAIL;
		}
		std::vector<unsigned char> byteCode(std::istreambuf_iterator<char>(fin), {});

		hr = _pd3dDevice->CreatePixelShader(&byteCode[0], byteCode.size(), nullptr, &_phongPS);
		if (FAILED(hr)) {
			return hr;
		}
	}
#endif		
	//D3D11_BUFFER_DESC bd = {};
	hr_ = CreateBuffers(&bd_);
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ManagersSetUp();

	MatrixIntialize();
	CameraSet();

	D3D11_TEXTURE2D_DESC ShadowTextureCreate;
	ZeroMemory(&ShadowTextureCreate, sizeof(ShadowTextureCreate));
	ShadowTextureCreate.Width = width_;
	ShadowTextureCreate.Height = height_;
	ShadowTextureCreate.MipLevels = 1;
	ShadowTextureCreate.ArraySize = 1;
	ShadowTextureCreate.Format = DXGI_FORMAT_R32_TYPELESS;
	ShadowTextureCreate.SampleDesc.Count = 1;
	ShadowTextureCreate.Usage = D3D11_USAGE_DEFAULT;
	ShadowTextureCreate.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ShadowTextureCreate.CPUAccessFlags = 0;
	ShadowTextureCreate.MiscFlags = 0;
	_pd3dDevice->CreateTexture2D(&ShadowTextureCreate, nullptr, &m_ShadowTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSV_desc = {};
	ZeroMemory(&shadowDSV_desc, sizeof(shadowDSV_desc));
	shadowDSV_desc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSV_desc.Texture2D.MipSlice = 0;
	_pd3dDevice->CreateDepthStencilView(m_ShadowTexture, &shadowDSV_desc, &m_shadowDSV);



	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRV_desc;
	shadowSRV_desc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowSRV_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowSRV_desc.Texture2D.MostDetailedMip = 0;
	shadowSRV_desc.Texture2D.MipLevels = 1;
	_pd3dDevice->CreateShaderResourceView(m_ShadowTexture, &shadowSRV_desc, &m_ShadowSRV);


	return static_cast<HRESULT>(0L);
}
void D3DFramework::MatrixIntialize()
{
	world_ = XMMatrixIdentity();
	plinth_ = XMMatrixIdentity();
	room_ = XMMatrixIdentity();
	model_ = XMMatrixIdentity();
}

void D3DFramework::CameraSet()
{
	auto lookat = XMFLOAT3(0, 0, 10);
	cam1.SetPosition(m_read_file->GetCamera1()[0]);
	cam1.SetLookAtPos(lookat);
	curCam = &cam1;
	view_ = curCam->GetViewMatrix();
	curCam->SetProjectionValues(XM_PIDIV2, width_ / static_cast<FLOAT>(height_), 0.01f, 100.0f);
	projection_ = curCam->GetProjectionMatrix();
	cam2.SetPosition(m_read_file->GetCamera2()[0]);
	lookat = XMFLOAT3(0, 0, 10);
	cam2.SetLookAtPos(lookat);
	light_pos_ = m_read_file->GetSpotLights();
	
	local_light_ = m_read_file->GetStaticLight();
	_lightProjection = XMMatrixPerspectiveLH(600, 800, 0.1f, 25.0f);
	CBChangeOnResize cbChangesOnResize{};
	cbChangesOnResize.mProjection = XMMatrixTranspose(projection_);
	_pImmediateContext->UpdateSubresource(g_pCBChangeOnResize, 0, nullptr, &cbChangesOnResize, 0, 0);

}

void D3DFramework::ManagersSetUp()
{
	m_modelManager = new ModelManager(_pd3dDevice, _pImmediateContext);

	dust_ = new Dust(_pd3dDevice.p, _pImmediateContext.p);
	textRend = new RenderTexture(_pd3dDevice, width_, height_, _pImmediateContext);
	gen_ = new GeneratePipeline(_pImmediateContext, _pd3dDevice);
	m_shaderManager = new ShaderManager(_pd3dDevice, _pImmediateContext, _hWnd, width_, height_);
	m_read_file = new FileRead();
	m_read_file->ReadFile("Config.txt");
}
float D3DFramework::TimeSet()
{

	float t = 0.0f;
	// Update our time
	if (_driverType == D3D_DRIVER_TYPE_REFERENCE) {
		t += static_cast<float>(XM_PI) * 0.0125f;
	}
	else {
		static ULONGLONG timeStart = 0;
		const ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = static_cast<float>(timeCur - timeStart) / time_scale_;
	}
	//DustMake(t);
	// Rotate the second light around the origin
	const float radius = 10.0f;
	// Clear the back buffer
	if (light_choice == 0)
	{
		/*const XMVECTOR light = XMLoadFloat4(&light_pos_[0]);
		XMStoreFloat4(local_light_, light);*/
		light_pos_[0] = local_light_;
	}
	else if (light_choice == 1)
	{
		light_pos_[0] = XMFLOAT4(sinf(t) * radius, 10, cosf(t) * radius, 1);
	}
	else if (light_choice == 2)
	{
		light_pos_[0] = XMFLOAT4(-10, 20, 10, 1);
	}

	const	XMVECTOR eye = XMLoadFloat4(&light_pos_[3]);
	_lightView = XMMatrixLookAtLH(eye, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));

	return t;
}
//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
D3DFramework::~D3DFramework() {

	delete dust_;
	delete m_shaderManager;
	delete m_modelManager;
	delete textRend;
	delete gen_;
	delete m_read_file;
}

HRESULT D3DFramework::CreateBuffers(D3D11_BUFFER_DESC* const bdin)
{
	bdin->Usage = D3D11_USAGE_DEFAULT;
	bdin->ByteWidth = sizeof(CBLight);
	bdin->BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bdin->CPUAccessFlags = 0;
	hr_ = _pd3dDevice->CreateBuffer(bdin, nullptr, &g_pCBLight);
	if (FAILED(hr_))
		return hr_;
	bdin->ByteWidth = sizeof(CBChangeOnResize);
	hr_ = _pd3dDevice->CreateBuffer(bdin, nullptr, &g_pCBChangeOnResize);
	if (FAILED(hr_))
		return hr_;
	bdin->ByteWidth = sizeof(CBChangesEveryFrame);
	hr_ = _pd3dDevice->CreateBuffer(bdin, nullptr, &g_pCBChangesEveryFrame);
	if (FAILED(hr_))
		return hr_;
	return hr_;
}

HRESULT D3DFramework::LightConstantBuffer()const
{
	const CBLight cb{ XMMatrixTranspose(_lightView),XMMatrixTranspose(_lightProjection) };
	_pImmediateContext->UpdateSubresource(g_pCBLight, 0, nullptr, &cb, 0, 0);
	if (FAILED(hr_))
		return hr_;
	return hr_;
}
//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void D3DFramework::Render() {

	time_ = TimeSet();
	_pImmediateContext->RSSetViewports(1, &vp_);
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, Colors::MidnightBlue);
	_pImmediateContext->ClearDepthStencilView(textRend->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	hr_ = LightConstantBuffer();
	RoomMake();
	if (models == 0)
	{
		model_ = XMMatrixTranslation(0, 4, 0);
	}
	if (models == 1)
	{
		model_ = XMMatrixScaling(20, 20, 20) * XMMatrixTranslation(0, 2, 0);
	}
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, textRend->GetDepthStencilView());
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(model_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBLight);
	_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	m_modelManager->ChooseModel(models);
	PipelineSet1(VSchoose);
	m_shaderManager->ShaderPass1Choice(VSchoose);
	m_modelManager->ModelIndex(models);
	if (ShaderManager::IsSecondPass(VSchoose))
	{
		m_shaderManager->ShaderPass2Choice(VSchoose);
		PipelineSet2(VSchoose);
		m_modelManager->ModelIndex(models);
	}
	FLOAT blend[4]{ 0,0,0,0 };
	gen_->SetBlendState("blendOff", blend);
	_swapChain->Present(0, 0);
}

void D3DFramework::RoomMake()
{
	const auto camX = XMVectorGetX(curCam->GetPosVector());
	const auto camY = XMVectorGetY(curCam->GetPosVector());
	const auto camZ = XMVectorGetZ(curCam->GetPosVector());
	if (camY < -1)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("glow_Blend", blend);
	}
	else
	{
		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	//floor
	room_ = XMMatrixScaling(5, 0.2, 5);
	 cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, textRend->GetDepthStencilView());
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBLight);
	_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	m_shaderManager->BasicShader();
	gen_->SetRasterState("solid_back_cullbias0");
	const auto a = gen_->GetSamplerState("sample_clamp");
	_pImmediateContext->PSSetSamplers(0, 1, &a);
	m_modelManager->SetModel("cube");
	_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	m_modelManager->SetIndexID("cube");
	////Ceiling
	if (camY > 10)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("glow_Blend", blend);
	}
	else
	{
		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	room_ = XMMatrixScaling(5, 0.2, 5) * XMMatrixTranslation(0, 10, 0);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	m_modelManager->SetIndexID("cube");
	////Wall +Z
	if (camZ > 5)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("glow_Blend", blend);
	}
	else
	{
		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	room_ = XMMatrixScaling(5, 5, 0.2) * XMMatrixTranslation(0, 5, 5);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	m_modelManager->SetIndexID("cube");
	//Wall -Z
	if (camZ < -5)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("glow_Blend", blend);
	}
	else
	{
		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	room_ = XMMatrixScaling(5, 5, 0.2) * XMMatrixTranslation(0, 5, -5);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	m_modelManager->SetIndexID("cube");
	//Wall -x
	if (camX < -5)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("transparent_blend", blend);
	}
	else
	{

		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	room_ = XMMatrixScaling(0.2, 5, 5) * XMMatrixTranslation(-5, 5, 0);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	m_modelManager->SetIndexID("cube");
	if (camX > 5)
	{
		FLOAT blend[4]{ 1,1,1,1 };
		gen_->SetBlendState("glow_Blend", blend);
	}
	else
	{
		FLOAT blend[4]{ 0,0,0,0 };
		gen_->SetBlendState("blendOff", blend);
	}
	//Wall +X
	room_ = XMMatrixScaling(0.2, 5, 5) * XMMatrixTranslation(5, 5, 0);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	m_modelManager->SetIndexID("cube");

	room_ = XMMatrixScaling(0.5, 1.5, 0.5) * XMMatrixTranslation(0, 1.4, 0);
	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(room_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),curCam->GetPositionFloat3(),XMFLOAT2(models,0) };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	FLOAT blend[4]{ 0,0,0,0 };
	gen_->SetBlendState("blendOff", blend);
	//	m_shaderManager->BumpMappingShaderSet();
	m_modelManager->SetIndexID("cube");
}
void D3DFramework::Shadow()
{
	//_pImmediateContext->OMSetRenderTargets(0, nullptr, m_shadowDSV);
	////_pImmediateContext->ClearDepthStencilView(m_shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);

	//_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBLight);
	//_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	//_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	//_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	////_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBLight);
	//m_shaderManager->ShadowShaderPass1();
	//m_modelManager->setModel("cube");
	//m_modelManager->setIndexID("cube");
	//_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, textRend->GetDepthStencilView());
	//const FLOAT clear[4] = { 0.6f, 0.6f, 0.6f, 0 };
	//auto a = gen->GetSamplerState("sample_wrap");
	//_pImmediateContext->PSSetSamplers(0, 1, &a);
	//a = gen->GetSamplerState("sample_Border");
	//_pImmediateContext->PSSetSamplers(2, 1, &a);
	//_pImmediateContext->PSSetSamplers(1, 1, &a);

	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	//_pImmediateContext->PSSetShaderResources(0, 1, &m_ShadowSRV);
	//m_shaderManager->ShadowShaderpass2();
	//m_modelManager->setIndexID("cube");
	//_Model = XMMatrixScaling(20, 0.2, 20);

	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	//m_modelManager->setIndexID("cube");
	//ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	//_pImmediateContext->PSSetShaderResources(0, 1, nullSRV);
	/*_pImmediateContext->RSSetViewports(1, &shadowVP);*/
	//_pImmediateContext->ClearDepthStencilView(textRend->GetShadowDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(model_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,models),curCam->GetPositionFloat3() };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	_pImmediateContext->OMSetRenderTargets(0, nullptr, textRend->GetShadowDSV());
	_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBLight);
	_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	m_shaderManager->ShadowShaderPass1();
	gen_->SetRasterState("solid_front_cull_bias0");
	m_modelManager->SetModel("cube");

	m_modelManager->SetIndexID("cube");
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, textRend->GetDepthStencilView());

	const auto a = gen_->GetSamplerState("sample_wrap");
	gen_->SetRasterState("solid_back_cull_bias0");
	const auto tex = textRend->GetShadowSRV();
	_pImmediateContext->PSSetSamplers(0, 1, &a);
	//_pImmediateContext->PSSetShaderResources(1, 1, &tex);
	m_shaderManager->ScreenTest();
	m_modelManager->SetIndexID("cube");
	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	//_pImmediateContext->RSSetViewports(1, &vp);

	//m_modelManager->chooseModel(models);
	//m_modelManager->modelIndex(models);
	//_pImmediateContext->ClearDepthStencilView(textRend->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	////second pass
	//m_shaderManager->ShadowShaderpass2();
	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	//_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, textRend->GetDepthStencilView());
	//const auto a = gen->GetSamplerState("sample_Border");
	//gen->SetRasterState("solid_back_cull_bias0");
	//const auto tex = textRend->GetShadowSRV();
	//_pImmediateContext->PSSetSamplers(1, 1, &a);
	//_pImmediateContext->PSSetShaderResources(1, 1, &tex);
	//_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	//m_modelManager->chooseModel(models);
	//m_modelManager->modelIndex(models);

	//_Model = XMMatrixScaling(20, 0.2, 20);

	//cb6 = { XMMatrixTranspose(_View), XMMatrixTranspose(_Model),lightPos[0],lightPos[1],lightPos[2],lightPos[3],XMFLOAT2(time,models),curCam->GetPositionFloat3() };
	//_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	//m_modelManager->setModel("cube");
	//m_modelManager->setIndexID("cube");
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	_pImmediateContext->PSSetShaderResources(1, 1, nullSRV);

}


void D3DFramework::DustMake(const float t) 
{
	 cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(world_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,models),curCam->GetPositionFloat3() };
	_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);
	dust_->RenderBuffers();
	_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBLight);
	_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	//const FLOAT clear[4] = { 0.0,0.0,0.0,0.0 };

	dust_->KillParticles();
	dust_->EmitParticles(t);
	dust_->UpdateParticles(t);
	const auto a = gen_->GetSamplerState("sample_wrap");

	dust_->UpdateBuffers();
	m_shaderManager->Dust2ShaderSet();
	//gen->SetRasterState("solid_none_cull_bias1");
	//_pImmediateContext->OMSetDepthStencilState(gen.GetDepthStencil("depthOn"), 0);
	_pImmediateContext->PSSetSamplers(0, 1, &a);
	_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	_pImmediateContext->DrawIndexed(dust_->GetIndexCount(), 0, 0);
}

void D3DFramework::UpdateView()
{
	curCam->UpdateViewMatrix();
	view_ = curCam->GetViewMatrix();
}


void D3DFramework::PipelineSet1(const int shader) const
{
	const auto a = gen_->GetSamplerState("sample_wrap");
	switch (shader)
	{
	case 2:
		_pImmediateContext->PSSetSamplers(0, 1, &a);
		break;
	case 3:
		//toon
		gen_->SetRasterState("solid_none_cull_bias1");
		break;
	case 4:
		gen_->setDepthStencil("Default");
		break;
		//transparent
	case 5:
		FLOAT blend[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
		gen_->SetBlendState("glow_Blend", blend);
		//gen->SetRasterState("solid_none_cull_bias1");
		break;
	}
}

void D3DFramework::PipelineSet2(const int shader)
{
	const auto a = gen_->GetSamplerState("sample_wrap");
	FLOAT clear[4] = { 0.0,0.0,0.0,0.0 };
	switch (shader)
	{
		//toon
	case 3:
		//gen->SetRasterState("wireframe_back_cull_bias0");
		break;
		//glowing
	case 4:
		float glowScale;

		if (_instance->models == 0)
		{
			glowScale = sin(time_);
			model_ = XMMatrixScaling(glowScale-0.3f, glowScale-0.3f, glowScale-0.3f) * XMMatrixTranslation(0, 4, 0);
		}
		if (_instance->models == 1)
		{
			glowScale = sin(time_) * 0.5 * 10;
			model_ = XMMatrixScaling(20 + glowScale, 20 + glowScale, 20 + glowScale) * XMMatrixTranslation(0, 2 - glowScale * 0.1, 0);
		}
		cb6 = { XMMatrixTranspose(view_), XMMatrixTranspose(model_),light_pos_[0],light_pos_[1],light_pos_[2],light_pos_[3],XMFLOAT2(time_,light_choice),cam1.GetPositionFloat3() };
		_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, nullptr, &cb6, 0, 0);

		_pImmediateContext->PSSetSamplers(0, 1, &a);
		gen_->setDepthStencil("BackStencil");
		break;
	default:
		break;
	}
}