#include "GeneratePipeline.h"

GeneratePipeline::GeneratePipeline(const CComPtr<ID3D11DeviceContext>& contex, const CComPtr<ID3D11Device>& devic):
	bd_(), init_data_(), device_(devic), context_(contex), depth_stencil_states_(), raster_states_(), sample_states_(),
	blend_sates_(),depth_stencil_views_(),  intialize_state_()
{

    D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc, blendStateDescription;
	ZeroMemory(&RenderTargetBlendDesc, sizeof(RenderTargetBlendDesc));
	RenderTargetBlendDesc.BlendEnable = true;
	RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_ONE;
	RenderTargetBlendDesc.DestBlend = D3D11_BLEND_ONE;
	RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.RenderTargetWriteMask = 0x0f;


	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(RenderTargetBlendDesc));
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0] = RenderTargetBlendDesc;
	AddBlendState(blendDesc, "glow_Blend");

	ZeroMemory(&RenderTargetBlendDesc, sizeof(RenderTargetBlendDesc));
	RenderTargetBlendDesc.BlendEnable = false;
	ZeroMemory(&blendDesc, sizeof(RenderTargetBlendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[3] = RenderTargetBlendDesc;
	AddBlendState(blendDesc, "blendOff");

	ZeroMemory(&blendStateDescription, sizeof(blendStateDescription));
	blendStateDescription.BlendEnable = true;
	blendStateDescription.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTargetWriteMask = 0x0f;

	
	ZeroMemory(&blendDesc, sizeof(blendStateDescription));
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[1] = blendStateDescription;
	AddBlendState(blendDesc, "transparent_blend");

	//Rasterizer
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.ScissorEnable = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	AddRasterState(rasterDesc, "solid_front_cull_bias0");
	rasterDesc.CullMode = D3D11_CULL_BACK;
	AddRasterState(rasterDesc, "solid_back_cull_bias0");
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	AddRasterState(rasterDesc, "wireframe_back_cull_bias0");

	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 1.0f;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 1.0f;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	AddRasterState(rasterDesc, "solid_none_cull_bias1");
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	AddRasterState(rasterDesc, "solid_front_cull_bias1");
	rasterDesc.CullMode = D3D11_CULL_BACK;
	AddRasterState(rasterDesc, "solid_back_cull_bias1");
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	AddRasterState(rasterDesc, "wireframe_back_cull_bias1");

	
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.ScissorEnable = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	AddRasterState(rasterDesc, "solid_none_cull_bias0");

	//Samplers
	D3D11_SAMPLER_DESC sampDesc = {};
	//ZeroMemory(&sampDesc, sizeof(sampDesc);
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	AddSampler(sampDesc, "sample_wrap");
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	AddSampler(sampDesc, "sample_clamp");

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1.0f;
	sampDesc.MinLOD = 0.f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MipLODBias = 0.f;
	sampDesc.MaxAnisotropy = 0;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	AddSampler(sampDesc, "sample_Border");

	//DepthStencil
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.StencilEnable = false;
	dsDesc.DepthEnable = false;
	addDepthStencilState(dsDesc, "depthOff");
	dsDesc.DepthEnable = true;
	addDepthStencilState(dsDesc, "depthOn");
	dsDesc.StencilEnable = true;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	addDepthStencilState(dsDesc, "Default");


	// Stencil operations if pixel is back-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	addDepthStencilState(dsDesc, "BackStencil");
}



void GeneratePipeline::AddRasterState(const D3D11_RASTERIZER_DESC& state, const std::string& name)
{
	device_->CreateRasterizerState(&state, &raster_states_[name]);
}

ID3D11RasterizerState* GeneratePipeline::GetRasterState(const std::string& name)
{
	return raster_states_[name];
}

void GeneratePipeline::SetRasterState(const std::string& name)
{
	context_->RSSetState(raster_states_[name].p);
}

void GeneratePipeline::addDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& state, const std::string& name)
{
	device_->CreateDepthStencilState(&state, &depth_stencil_states_[name]);
}

ID3D11DepthStencilState* GeneratePipeline::GetDepthStencil(const std::string& name)
{
	return depth_stencil_states_[name];
}

void GeneratePipeline::setDepthStencil(const std::string& name)
{
	context_->OMSetDepthStencilState(depth_stencil_states_[name], 0);
}

void GeneratePipeline::AddSampler(const D3D11_SAMPLER_DESC& state, const std::string& name)
{
	device_->CreateSamplerState(&state, &sample_states_[name]);
}


ID3D11SamplerState* GeneratePipeline::GetSamplerState(const std::string& name)
{
	return sample_states_[name];
}

void GeneratePipeline::AddBlendState(const D3D11_BLEND_DESC& state, const std::string& name)
{
	device_->CreateBlendState(&state, &blend_sates_[name]);
}

ID3D11BlendState* GeneratePipeline::GetBlend(const std::string& name)
{
	return blend_sates_[name];
}

void GeneratePipeline::SetBlendState(const std::string& name, const FLOAT* const blendFact)
{
	context_->OMSetBlendState(blend_sates_[name], blendFact, 1);
}

