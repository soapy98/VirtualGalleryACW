#pragma once
#include <directxmath.h>
#include <unordered_map> 

#include "Mesh.h"
#include "SimpleVertex.h"

using namespace DirectX;
template<typename T1> using Meshmap = std::unordered_map<std::string, Mesh>;
class ModelManager
{
public:
	ModelManager( ID3D11Device* d, ID3D11DeviceContext* c);
	~ModelManager()=default;

	void LoadDust();
	
	//Mesh GenerateModel();

	void SetModel(const std::string& name);

	void AddModel(const std::string& name);

	void SetIndexID(const std::string& name);

	ModelManager(ModelManager&)=default;
	ModelManager(ModelManager&&)noexcept = default;
	ModelManager& operator=(const ModelManager&) = default;
	ModelManager& operator=(const ModelManager&&) noexcept;

	void ChooseModel(const int model);
	void ModelIndex(const int model);
	void LoadPlinth();

	void LoadRoom();

private:
	ID3D11Device* device_;
	ID3D11DeviceContext* context_;
	Meshmap<Mesh> model_map_;
};

