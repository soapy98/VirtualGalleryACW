#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <unordered_map> 
#include <atlcomcli.h>
#include "Textures.h"

using namespace DirectX;
template<typename T1> using shader_map = std::unordered_map<std::string, CComPtr<T1>>;
class Shaders
{
public:
	Shaders(ID3D11Device*d, ID3D11DeviceContext* c);
	Shaders() = default;;
	~Shaders() = default;;
	Shaders(const Shaders& other) = default;;

	Shaders(Shaders&& other) noexcept=default;
	Shaders& operator=(const Shaders& other) = default;

	Shaders& operator=(Shaders&& other) noexcept = default;
	void AddPS(const std::string& name);
	ID3D11PixelShader* GetPixel(const std::string& name);
	void SetPixel(const std::string& name);

	void AddVS(const std::string& name);
	ID3D11VertexShader* GetVertex(const std::string& name);
	void SetVertex(const std::string& name);
	bool GetIntializeState()const { return intialize_state_; }
	void SetShader();
private:
	
	shader_map<ID3D11VertexShader> vertex_shaders_{};
	shader_map<ID3D11PixelShader> pixel_shaders_{};
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;
	bool intialize_state_{};
};

