#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
struct DustVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT4 Col;
};