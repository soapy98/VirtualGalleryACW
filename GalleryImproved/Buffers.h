#include "D3DFramework.h"

class Buffers
{
	int IndexSize;
	int vertexSize;
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA InitData = {};
	
public:
	Buffers();
	~Buffers();

private:

};

Buffers::Buffers()
{
	
}

Buffers::~Buffers()
{
}