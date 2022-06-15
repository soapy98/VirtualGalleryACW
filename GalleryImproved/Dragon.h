#pragma once
#include <d3d11.h>

struct CBChangesEveryFrame;

class Dragon
{
	char* model;
public:
	Dragon();
	void Render(CBChangesEveryFrame& cb, ID3D11DeviceContext* _im, ID3D11Buffer* buff, ID3D11PixelShader* PS, ID3D11VertexShader* vs);
	void vertices(char* modelname, int vertSize, int indiciesSize);
};
