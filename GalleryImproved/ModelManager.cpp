#include <assimp/Importer.hpp>
#include <assimp/scene.h>         
#include <assimp/postprocess.h>

#include "ModelManager.h"
ModelManager::ModelManager(ID3D11Device* const d, ID3D11DeviceContext* const c) :device_(d), context_(c)
{
	AddModel("Models/Sphere3.obj");
	AddModel("Models/dragonV3.ply");
	LoadPlinth();
}

//void ModelManager::LoadDust()
//{
//	/*const int numParticles = 40;
//	float particleID;
//	SimpleVertex vertices[numParticles * 4];
//	for (int i = 0; i < numParticles; i++)
//	{
//		particleID = static_cast<float>(i / numParticles);
//		vertices[i * 4] = { XMFLOAT3(-1.0f,-1.0f,particleID),XMFLOAT2(1.0f,0.0f) ,XMFLOAT3(0.0f,0.0f,0.0f) };
//		vertices[i * 4 + 1] = { XMFLOAT3(1.0f,-1.0f,particleID),XMFLOAT2(0.0f,0.0f) ,XMFLOAT3(0.0f,0.0f,0.0f) };
//		vertices[i * 4 + 2] = { XMFLOAT3(1.0f,1.0f,particleID),XMFLOAT2(0.0f,1.0f),XMFLOAT3(0.0f,0.0f,0.0f) };
//		vertices[i * 4 + 3] = { XMFLOAT3(-1.0f,1.0f,particleID),XMFLOAT2(1.0f,1.0f),XMFLOAT3(0.0f,0.0f,0.0f) };
//
//	}
//	D3D11_BUFFER_DESC bd = {};
//	bd.Usage = D3D11_USAGE_DYNAMIC;
//	bd.ByteWidth = sizeof(SimpleVertex) * 36;
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData = {};
//	InitData.pSysMem = vertices;
//	device->CreateBuffer(&bd, &InitData, &models["Dust"]._vertexBuffer);
//	WORD indices[numParticles * 6];
//	for (unsigned short i = 0; i < numParticles; i++)
//	{
//		indices[i * 6] = i * 4 + 2;
//		indices[i * 6 + 1] = i * 4;
//		indices[i * 6 + 2] = i * 4 + 1;
//		indices[i * 6 + 3] = i * 4 + 3;
//		indices[i * 6 + 4] = i * 4;
//		indices[i * 6 + 5] = i * 6 + 2;
//	}
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(WORD) * numParticles * 6;
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	InitData.pSysMem = indices;
//	device->CreateBuffer(&bd, &InitData, &models["Dust"]._indexBuffer);
//	models["Dust"]._numofIndicies = numParticles * 6;
//	models["Dust"]._numofVerts = 8;*/
//}

void ModelManager::SetModel(const std::string& name)
{
	const	auto stride = static_cast<UINT>(sizeof(SimpleVertex));
	const	auto offset = static_cast<UINT>(0);
	this->context_->IASetVertexBuffers(0, 1, &model_map_[name]._vertexBuffer.p, &stride, &offset);
	this->context_->IASetIndexBuffer(model_map_[name]._indexBuffer, DXGI_FORMAT_R16_UINT, 0);

}
void ModelManager::AddModel(const std::string& name)
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(name, aiPrimitiveType_TRIANGLE | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);
	const aiMesh* const teapotMesh = scene->mMeshes[0];
	std::vector<SimpleVertex>mesh_vertices;

	for (UINT i = 0; i < teapotMesh->mNumVertices; i++) {
		SimpleVertex vertex;
		vertex.Pos.x = teapotMesh->mVertices[i].x;
		vertex.Pos.y = teapotMesh->mVertices[i].y;
		vertex.Pos.z = teapotMesh->mVertices[i].z;
		vertex.Normal.x = teapotMesh->mNormals[i].x;
		vertex.Normal.y = teapotMesh->mNormals[i].y;
		vertex.Normal.z = teapotMesh->mNormals[i].z;
		mesh_vertices.push_back(vertex);
	}
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mesh_vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = mesh_vertices.data();
	device_->CreateBuffer(&bd, &InitData, &model_map_[name]._vertexBuffer.p);

	std::vector<WORD>mesh_indices;
	for (UINT i = 0; i < teapotMesh->mNumFaces; i++) {
		aiFace const face = teapotMesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			mesh_indices.push_back(face.mIndices[j]);
	}
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * mesh_indices.size();        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = mesh_indices.data();
	device_->CreateBuffer(&bd, &InitData, &model_map_[name]._indexBuffer);
	model_map_[name]._numofIndicies = mesh_indices.size();
	model_map_[name]._numofVerts = mesh_vertices.size();
}

void ModelManager::SetIndexID(const std::string& name)
{
	this->context_->DrawIndexed(model_map_[name]._numofIndicies, 0, 0);
}

void ModelManager::ChooseModel(const int model)
{
	switch (model)
	{
	case 0:
		SetModel("Models/Sphere3.obj");
		break;
	case 1:
		SetModel("Models/dragonV3.ply");
		break;
	default:
		break;
	}

}
void ModelManager::ModelIndex(const int model)
{
	switch (model)
	{
	case 0:
		SetIndexID("Models/Sphere3.obj");
		break;
	case 1:
		SetIndexID("Models/dragonV3.ply");
		break;
	default:
		break;
	}

}

void ModelManager::LoadPlinth()
{
	std::vector<SimpleVertex>vertices
	{
	SimpleVertex{XMFLOAT3{1.000000,1.000000,-1.000000},XMFLOAT3{0.333333,0.666667,-0.666667}},
	SimpleVertex{XMFLOAT3{-1.00,1.000000,-1.000000} ,XMFLOAT3{-0.816497,0.408248,-0.408248}},    // Vertex 1.
	SimpleVertex{ XMFLOAT3{-1.00,1.000000,1.000000},XMFLOAT3{	-0.333333,0.666667,0.666667}},     // And so on.
	SimpleVertex{ XMFLOAT3{1.00,1.000000,1.000000}	,XMFLOAT3{	0.816497,0.408248,0.408248}},
	SimpleVertex{ XMFLOAT3{1.00,-1.000000,-1.000000},XMFLOAT3{0.666667,-0.666667,-0.333333,}},
	SimpleVertex{ XMFLOAT3{-1.00,-1.000000,-1.000000},XMFLOAT3{-0.408248,-0.408248,-0.816497}},
	SimpleVertex{ XMFLOAT3{-1.0,-1.000000,1.000000},XMFLOAT3{-0.666667,-0.666667,0.333333}},
	SimpleVertex{ XMFLOAT3{1.0,-1.000000,1.000000},XMFLOAT3{0.408248,-0.408248,0.816497}},

	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices.data();
	device_->CreateBuffer(&bd, &InitData, &model_map_["cube"]._vertexBuffer);
	std::vector<WORD> indices{ 0,1,2,
								0,2,3,
								0,4,5,
								0,5,1,
								1,5,6,
								1,6,2,
								2,6,7,
								2,7,3,
								3,7,4,
								3,4,0,
								4,7,6,
								4,6,5 };

	//indices.push_back(0); // top
	//indices.push_back(1);
	//indices.push_back(2);
	//indices.push_back(2);
	//indices.push_back(1);
	//indices.push_back(3);


	//indices.push_back(4); // bot
	//indices.push_back(6);
	//indices.push_back(5);
	//indices.push_back(6);
	//indices.push_back(7);
	//indices.push_back(5);



	//indices.push_back(8); // front
	//indices.push_back(10);
	//indices.push_back(9);

	//indices.push_back(10);
	//indices.push_back(11);
	//indices.push_back(9);

	//indices.push_back(12); // back
	//indices.push_back(13);
	//indices.push_back(14);

	//indices.push_back(14);
	//indices.push_back(13);
	//indices.push_back(15);



	//indices.push_back(16); // left
	//indices.push_back(17);
	//indices.push_back(18);



	//indices.push_back(18);
	//indices.push_back(17);
	//indices.push_back(19);



	//indices.push_back(20); // left
	//indices.push_back(22);
	//indices.push_back(21);

	//indices.push_back(22);
	//indices.push_back(23);
	//indices.push_back(21);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indices.size();       // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	device_->CreateBuffer(&bd, &InitData, &model_map_["cube"]._indexBuffer);
	model_map_["cube"]._numofIndicies = indices.size();
	model_map_["cube"]._numofVerts = vertices.size();
}



