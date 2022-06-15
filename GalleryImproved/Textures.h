#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
#include <vector>
#include <string>
#include <atlcomcli.h>
#include <unordered_map>
#include "DDSTextureLoader.h"

using namespace DirectX;
template<typename T1> using Rmap = std::unordered_map<std::string, CComPtr<T1>>;
class Texture
{
public:
	Texture()=default; // Default Constructor
	Texture(ID3D11Device* dev, ID3D11DeviceContext* );
	Texture(const Texture& other) = default;; // Copy Constructor
	Texture(Texture&& other) noexcept = default; // Move Constructor

	Texture& operator = (const Texture& other) = default;; // Copy Assignment Operator
	Texture& operator = (Texture&& other) noexcept = default;; // Move Assignment Operator
	~Texture() = default; // Destructor

	void LoadAllTextures();
	bool GetInitializeState() const
	{
		return m_initialization_;
	}
	ID3D11ShaderResourceView* GetTexture(const std::string& name);
	void SetTexture(const int pass,const std::string& name);
private:
	Rmap<ID3D11ShaderResourceView>resources;
	ID3D11Device* device= nullptr;
	ID3D11DeviceContext* context = nullptr;
	bool m_initialization_={};
};