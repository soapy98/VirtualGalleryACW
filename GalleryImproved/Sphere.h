#pragma once
#include <d3d11_1.h>



struct CBChangesEveryFrame;

class Sphere
{
	
public:

	Sphere();
	explicit Sphere(CBChangesEveryFrame* cb);

	void Render(CBChangesEveryFrame& cb, ID3D11DeviceContext* _im, ID3D11Buffer* buff,ID3D11Buffer*buff2,ID3D11Buffer* buff3, ID3D11PixelShader* PS, ID3D11VertexShader* vs);


};






