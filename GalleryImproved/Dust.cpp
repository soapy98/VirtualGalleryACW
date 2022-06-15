#include "Dust.h"
//Based off Rastertex particle system
Dust::Dust(ID3D11Device* const devic, ID3D11DeviceContext* const contex) :device(devic), context(contex)
{

	Initialize();
	IntializeBuffer();
}


Dust::Dust()
{
}


Dust::~Dust()
{
	delete[] m_particleList;
	delete[] m_vertex;
}
bool Dust::Initialize()
{
	// Initialize the particle system.
	return  InitializeParticleSystem();
}
bool Dust::InitializeParticleSystem()
{
	int i;

	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 5.f;
	m_particleDeviationY = 5.f;
	m_particleDeviationZ = 2.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 5000;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}

void Dust::ShutdownParticleSystem()
{
	// Release the particle list.
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = nullptr;
	}

	return;
}

void Dust::IntializeBuffer()
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertex = new DustVertex[m_vertexCount];


	// Create the index array.
	indices = new unsigned long[m_indexCount];


	// Initialize vertex array to zeros at first.
	memset(m_vertex, 0, (sizeof(DustVertex) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(DustVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertex;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);


	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);


	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = nullptr;


}

void Dust::EmitParticles(const float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) * m_particleDeviationX;
		positionY = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) * m_particleDeviationY;
		positionZ = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) * m_particleDeviationZ;

		velocity = m_particleVelocity + ((static_cast<float>(rand()) - (static_cast<float>(rand()) / RAND_MAX))) * m_particleVelocityVariation;

		red = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) + 0.5f;
		green = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) + 0.5f;
		blue = ((static_cast<float>(rand()) - (static_cast<float>(rand())) / RAND_MAX)) + 0.5f;

		// Now generate the randomized particle properties.


		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

	return;
}
void Dust::UpdateParticles(const float frameTime)
{
	int i;
	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}


void Dust::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -10.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < m_maxParticles - 1; j++)
			{
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}

	return;
}
bool Dust::UpdateBuffers()
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DustVertex* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertex, 0, (sizeof(DustVertex) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i < m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(0.0f, 1.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(0.0f, 0.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(1.0f, 1.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(1.0f, 1.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(0.0f, 0.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertex[index].Pos = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertex[index].Tex = XMFLOAT2(1.0f, 0.0f);
		m_vertex[index].Col = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = static_cast<DustVertex*>(mappedResource.pData);

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, m_vertex, (sizeof(DustVertex) * m_vertexCount));

	// Unlock the vertex buffer.
	context->Unmap(m_vertexBuffer, 0);

	return true;
}
void Dust::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(DustVertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


int Dust::GetIndexCount()const
{
	return m_indexCount;
}
