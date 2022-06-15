#pragma once
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};