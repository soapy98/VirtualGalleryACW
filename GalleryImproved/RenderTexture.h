#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class RenderTexture
{
public:
	RenderTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight, ID3D11DeviceContext* cont);
	RenderTexture(const RenderTexture& other); // Copy Constructor
	RenderTexture(RenderTexture&& other) noexcept; // Move Constructor
	~RenderTexture();

	RenderTexture& operator = (const RenderTexture& other); // Copy Assignment Operator
	RenderTexture& operator = (RenderTexture&& other) noexcept; // Move Assignment Operator

	bool RenderObjectsToTexture(ID3D11RenderTargetView* RTV, ID3D11DepthStencilView* const depthStencilView) const;


	bool RenderObjectsToTexture(ID3D11DepthStencilView* const depthStencilView) const;

	void SetRenderTarget(ID3D11RenderTargetView* RTV, ID3D11DepthStencilView* const depthStencilView) const;

	void ClearRenderTarget(ID3D11RenderTargetView* RTV, ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4& RGBA) const;

	ID3D11ShaderResourceView* GetShaderResourceView() const;

	ID3D11RenderTargetView* GetTargetView() const;

	ID3D11DepthStencilView* GetDepthStencilView() const;

	ID3D11ShaderResourceView* GetShadowSRV() const;

	ID3D11RenderTargetView* GetShadowTargetView() const;

	ID3D11DepthStencilView* GetShadowDSV() const;




	bool GetInitializationState() const;

private:
	

	bool m_initializationFailed;

	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11Texture2D* m_ShadowTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11RenderTargetView* m_shadowTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11ShaderResourceView* m_ShadowSRV;
	ID3D11DepthStencilView* m_depthStencilView; 
	ID3D11DepthStencilView* m_shadowDSV;
	ID3D11DepthStencilState* m_depthStencil;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};

