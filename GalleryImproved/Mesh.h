#pragma once
#include <atlcomcli.h>
#include <d3d11_1.h>
using namespace DirectX;
struct Mesh
{
	unsigned int _strides;
	unsigned int _offset;
	unsigned int _numofVerts;
	unsigned int _numofIndicies;
	CComPtr<ID3D11Buffer>_vertexBuffer;
	CComPtr<ID3D11Buffer> _indexBuffer;

	Mesh() = default;
	Mesh(const unsigned int vertNum, const unsigned int indexNum);
	Mesh& operator=(const Mesh& other) = default;
};