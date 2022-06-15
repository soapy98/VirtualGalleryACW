#include "Sphere.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <atlbase.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
Sphere::Sphere()
{
}

Sphere::Sphere(CBChangesEveryFrame* cb)
{
}



void Sphere::Render(CBChangesEveryFrame& cb, ID3D11DeviceContext* _im,ID3D11Buffer* buff, ID3D11Buffer* buff2, ID3D11Buffer* buff3,ID3D11PixelShader* PS, ID3D11VertexShader*vs)
{
	
	_im->UpdateSubresource(buff3, 0, nullptr, &cb, 0, 0);
	_im->VSSetShader(vs, nullptr, 0);
	_im->VSSetConstantBuffers(0, 1, &buff);
	_im->VSSetConstantBuffers(1, 1, &buff2);
	_im->VSSetConstantBuffers(2, 1, &buff3);
	_im->PSSetShader(PS, nullptr, 0);
	_im->DrawIndexed(1000, 0, 0);
}


