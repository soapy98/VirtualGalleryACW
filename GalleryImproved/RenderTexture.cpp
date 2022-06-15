#include "RenderTexture.h"

RenderTexture::RenderTexture(ID3D11Device* const devic, const int textureWidth, const int textureHeight,
                             ID3D11DeviceContext* const cont) : m_initializationFailed(false),
                                                                m_renderTargetTexture(nullptr),
                                                                m_renderTargetView(nullptr), m_shadowTargetView(nullptr),
                                                                m_shaderResourceView(nullptr),m_depthStencil(), device(devic),
                                                                context(cont)
                                                               
{
	D3D11_TEXTURE2D_DESC descDepth = {};
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = textureWidth;
	descDepth.Height = textureHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags =  D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	 device->CreateTexture2D(&descDepth, nullptr, &m_renderTargetTexture);

	 D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	 ZeroMemory(&descDSV, sizeof(descDSV));
	 descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	 descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	 descDSV.Texture2D.MipSlice = 0;
	 device->CreateDepthStencilView(m_renderTargetTexture, &descDSV, &m_depthStencilView);

	

	 D3D11_SHADER_RESOURCE_VIEW_DESC rend_SRV_Desc;
	 rend_SRV_Desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	 rend_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	 rend_SRV_Desc.Texture2D.MostDetailedMip = 0;
	 rend_SRV_Desc.Texture2D.MipLevels = 1;
	 device->CreateShaderResourceView(m_renderTargetTexture, &rend_SRV_Desc, &m_shaderResourceView);
	
	D3D11_TEXTURE2D_DESC ShadowTextureCreate;
	ZeroMemory(&ShadowTextureCreate, sizeof(ShadowTextureCreate));
	ShadowTextureCreate.Width = textureWidth;
	ShadowTextureCreate.Height = textureHeight;
	ShadowTextureCreate.MipLevels = 1;
	ShadowTextureCreate.ArraySize = 1;
	ShadowTextureCreate.Format = DXGI_FORMAT_R32_TYPELESS;
	ShadowTextureCreate.SampleDesc.Count = 1;
	ShadowTextureCreate.Usage = D3D11_USAGE_DEFAULT;
	ShadowTextureCreate.BindFlags = D3D11_BIND_DEPTH_STENCIL| D3D11_BIND_SHADER_RESOURCE;
	ShadowTextureCreate.CPUAccessFlags = 0;
	ShadowTextureCreate.MiscFlags = 0;
	device->CreateTexture2D(&ShadowTextureCreate, nullptr, &m_ShadowTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSV_desc = {};
	ZeroMemory(&shadowDSV_desc, sizeof(shadowDSV_desc));
	shadowDSV_desc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSV_desc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(m_ShadowTexture, &shadowDSV_desc, &m_shadowDSV);



	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRV_desc;
	shadowSRV_desc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowSRV_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowSRV_desc.Texture2D.MostDetailedMip = 0;
	shadowSRV_desc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_ShadowTexture, &shadowSRV_desc, &m_ShadowSRV);

}


RenderTexture::RenderTexture(const RenderTexture& other) = default;

RenderTexture::RenderTexture(RenderTexture && other) noexcept = default;

RenderTexture::~RenderTexture()
{
	try
	{
		if (m_shaderResourceView)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = nullptr;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
		}

		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = nullptr;
		}
	}
	catch (exception& e)
	{

	}
}

RenderTexture& RenderTexture::operator=(const RenderTexture & other) = default;

RenderTexture& RenderTexture::operator=(RenderTexture && other) noexcept = default;

bool RenderTexture::RenderObjectsToTexture(ID3D11RenderTargetView* const RTV,ID3D11DepthStencilView* const depthStencilView) const
{
	SetRenderTarget( RTV,depthStencilView);

	ClearRenderTarget(RTV,depthStencilView, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


	return true;
}

void RenderTexture::SetRenderTarget(ID3D11RenderTargetView*const RTV,ID3D11DepthStencilView* const depthStencilView) const {

	if (RTV == nullptr)
		context->OMSetRenderTargets(1, nullptr, depthStencilView);
	else
		context->OMSetRenderTargets(1, &RTV, depthStencilView);
}

void RenderTexture::ClearRenderTarget(ID3D11RenderTargetView*const RTV,ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4 & RGBA) const
{
	const float colour[4]{ RGBA.x, RGBA.y, RGBA.z, RGBA.w };

	context->ClearRenderTargetView(RTV, colour);

	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() const {
	return m_shaderResourceView;
}
ID3D11RenderTargetView* RenderTexture::GetTargetView()const {
	return m_renderTargetView;
}

ID3D11DepthStencilView* RenderTexture::GetDepthStencilView() const
{
	return m_depthStencilView;
}

ID3D11ShaderResourceView* RenderTexture::GetShadowSRV() const
{
	return m_ShadowSRV;
}

ID3D11RenderTargetView* RenderTexture::GetShadowTargetView() const
{
	return m_shadowTargetView;
}

ID3D11DepthStencilView* RenderTexture::GetShadowDSV() const
{
	return m_shadowDSV;
}

bool RenderTexture::GetInitializationState() const {
	return m_initializationFailed;
}
