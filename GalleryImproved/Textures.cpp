#include "Textures.h"
#include "DDSTextureLoader.h"


Texture::Texture(ID3D11Device*const dev, ID3D11DeviceContext * const con) :device(dev), context(con),m_initialization_(false)
{
}

void Texture::LoadAllTextures() {
	CreateDDSTextureFromFile(device, L"DDS files\\FireBase.dds", nullptr, &resources["FireBase"]);
	CreateDDSTextureFromFile(device, L"DDS files\\ScaleCol.dds", nullptr, &resources["ScaleColor"]);
	CreateDDSTextureFromFile(device, L"DDS files\\Emmision.dds", nullptr, &resources["ScaleEmmision"]);
	CreateDDSTextureFromFile(device, L"DDS files\\Height.dds", nullptr, &resources["ScaleHeight"]);
	CreateDDSTextureFromFile(device, L"DDS files\\Normal.dds", nullptr, &resources["ScaleNorm"]);
	CreateDDSTextureFromFile(device, L"DDS files\\Roughness.dds", nullptr, &resources["ScaleRough"]);
	CreateDDSTextureFromFile(device, L"DDS files\\Ambient.dds", nullptr, &resources["ScaleAmbient"]);
	CreateDDSTextureFromFile(device, L"DDS files\\FireBase.dds", nullptr, &resources["FireBaseCol"]);
	CreateDDSTextureFromFile(device, L"DDS files\\FireDistortion.dds", nullptr, &resources["FireDistort"]);
	CreateDDSTextureFromFile(device, L"DDS files\\FireOpacity.dds", nullptr, &resources["FireOpacity"]);
}

ID3D11ShaderResourceView* Texture::GetTexture(const std::string& name)
{
	return resources[name];
}
void Texture::SetTexture(const int pass,const std::string& name)
{
	context->PSSetShaderResources(pass, 1, &resources[name].p);
}


