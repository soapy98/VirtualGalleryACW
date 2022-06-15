#pragma once

#include <atlcomcli.h>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <unordered_map> 
using namespace DirectX;

template<typename T1> using shader_map = std::unordered_map<std::string, CComPtr<T1>>;

class GeneratePipeline
{
private:
	D3D11_BUFFER_DESC bd_ = {};
	D3D11_SUBRESOURCE_DATA init_data_ = {};
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;
	shader_map<ID3D11DepthStencilState> depth_stencil_states_;
	shader_map<ID3D11RasterizerState> raster_states_;
	shader_map<ID3D11SamplerState> sample_states_;
	shader_map<ID3D11BlendState> blend_sates_;
	shader_map<ID3D11DepthStencilView>depth_stencil_views_;
	bool intialize_state_;

public:
	GeneratePipeline() = default;

	GeneratePipeline(const CComPtr<ID3D11DeviceContext>& contex, const CComPtr<ID3D11Device>& devic);

	void AddRasterState(const D3D11_RASTERIZER_DESC& state, const std::string& name);
	ID3D11RasterizerState* GetRasterState(const std::string& name);
	void SetRasterState(const std::string& name);

	void addDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& state, const std::string& name);
	ID3D11DepthStencilState* GetDepthStencil(const std::string& name);
	void setDepthStencil(const std::string& name);

	void AddSampler(const D3D11_SAMPLER_DESC& state, const std::string& name);
	ID3D11SamplerState* GetSamplerState(const std::string& name);

	void AddBlendState(const D3D11_BLEND_DESC& state, const std::string& name);
	ID3D11BlendState* GetBlend(const std::string& name);
	void SetBlendState(const std::string& name, const FLOAT* const blendFact);

	void AddDepthStencilView(D3D11_DEPTH_STENCIL_VIEW_DESC& depth, const std::string& name);
	ID3D11DepthStencilView* GetDepthView(const std::string& name);
	void SetDepthStencilView(const std::string& name);

	GeneratePipeline(const GeneratePipeline& other)=default; // Copy Constructor
	GeneratePipeline(GeneratePipeline&& other) noexcept = default; // Move Constructor

	GeneratePipeline& operator = (const GeneratePipeline& other) = default;; // Copy Assignment Operator
	GeneratePipeline& operator = (GeneratePipeline&& other) noexcept = default;; // Move Assignment Operator

	bool GetIntializeState() const { return intialize_state_; };

	
};
