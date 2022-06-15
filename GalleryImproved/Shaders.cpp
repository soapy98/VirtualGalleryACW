#include "Shaders.h"
#include "D3DFramework.h"

Shaders::Shaders(ID3D11Device*const d, ID3D11DeviceContext*const c):device_(d),context_(c)
{

}

void Shaders::AddPS(const std::string& name)
{
	CComPtr <ID3DBlob> pVSBlob;
	D3DFramework::CompileShaderFromFile(L"Gallery.fx", name.c_str(), "ps_4_0", &pVSBlob);
	this->device_->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pixel_shaders_[name]);
}

ID3D11PixelShader* Shaders::GetPixel(const std::string& name)
{
	return pixel_shaders_[name];
}

void Shaders::SetPixel(const std::string& name)
{
	this->context_->PSSetShader(pixel_shaders_[name].p, nullptr, 0);
}

void Shaders::AddVS(const std::string& name)
{
	CComPtr <ID3DBlob> pVSBlob;
	D3DFramework::CompileShaderFromFile(L"Gallery.fx", name.c_str(), "vs_4_0", &pVSBlob);
	device_->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertex_shaders_[name]);
}


ID3D11VertexShader* Shaders::GetVertex(const std::string& name)
{
	return vertex_shaders_[name];
}

void Shaders::SetVertex(const std::string& name)
{
	this->context_->VSSetShader(vertex_shaders_[name].p,nullptr,0);
}

void Shaders::SetShader()
{
	AddVS("VS");
	AddPS("PS");
	AddPS("BasicPS");
	AddPS("BumpMap");

	AddPS("PhongPS");

	AddPS("ToonPS");
	AddVS("OutlineVS");
	AddPS("OutlinePS");


	AddVS("GouroudVS");
	AddPS("GouroudPS");


	AddPS("FirePS1");

	AddPS("ShadowPS1");
	AddPS("ShadowPS2");
	AddVS("ShadowVS1");
	AddVS("ShadowVS2");
	

	AddVS("DustVS");
	AddPS("DustPS");

	AddVS("ParticleVertexShader");
	AddPS("ParticlePixelShader");

	AddVS("ScreenVS");

	AddPS("transPS");


	AddPS("HDRPS");


}


