#pragma once
#include <d3d11.h>
#include "RenderTexture.h"
#include "Shaders.h"
#include "Textures.h"
#include <atlcomcli.h>

using namespace std;
using namespace DirectX;
class ShaderManager
{
public:
	void ToonShaderSet1()const;
	void ToonShaderSet2()const;
	void DustShaderSet()const;
	void Dust2ShaderSet()const;
	void BasicShader()const;
	void HDR()const;
	~ShaderManager();
	void ScreenTest()const;
	ShaderManager() = default;
	ShaderManager(CComPtr<ID3D11Device>& dev, CComPtr<ID3D11DeviceContext>& cont, HWND const hwnd, const int textureWidth, const int textureHeight);
	ShaderManager(const ShaderManager& other); // Copy Constructor
	ShaderManager(ShaderManager&& other) noexcept; // Move Constructor

	ShaderManager& operator = (const ShaderManager& other); // Copy Assignment Operator
	ShaderManager& operator = (ShaderManager&& other) noexcept; // Move Assignment Operator

	void ShaderPass1Choice(const int choice) const;

	void ShaderPass2Choice(const int choice) const;

	void PhongShaderSet() const;
	void GouroudShaderSet()const;
	void GlowingShader1Set()const;
	void GlowingShader2Set()const;
	void UnbindShaderResource(const int ShaderPass, ID3D11RenderTargetView*const RTV)const;
	void BumpMappingShaderSet()const;
	static bool IsSecondPass(const int pass);
	void TransparencyShaderSet()const;
	void TransparencyShaderSet2();
	void EnvironmentMapShaderSet();

	void ShadowShaderPass1() const;
	void ShadowShaderPass2() const;

	bool GetIntializeState() const { return m_intialize_state_; }
private:
	ID3D11Device* device_{};
	ID3D11DeviceContext* context_ = nullptr;
	bool m_intialize_state_{};
	Shaders* m_shader{};
	Texture* m_texture{};
	ID3D11ShaderResourceView* resource_{};
	ID3D11SamplerState* sample_state{};
	ID3D11DepthStencilView* _depth_stencil_view{};
	ID3D11RenderTargetView* _render_target_view{};
};

