#include "Dragon.h"

#include <vector>



struct SimpleVertex
{
	
};

Dragon::Dragon()
{
}

void Dragon::Render(CBChangesEveryFrame& cb, ID3D11DeviceContext* _im, ID3D11Buffer* buff, ID3D11PixelShader* PS, ID3D11VertexShader* vs)
{
	_im->UpdateSubresource(buff, 0, nullptr, &cb, 0, 0);
	_im->VSSetShader(vs, nullptr, 0);
	_im->VSSetConstantBuffers(0, 1, &buff);
	_im->PSSetShader(PS, nullptr, 0);
	_im->DrawIndexed(36, 0, 0);
}


