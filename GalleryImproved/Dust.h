#pragma once
#include <atlcomcli.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "Mesh.h"
#include <atlcomcli.h>
#include "ParticleStruct.h"
#include "RenderTexture.h"
#include "ModelManager.h"
class Dust
{
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};
public:
	~Dust();
	bool Initialize();
	Dust(ID3D11Device* devic,ID3D11DeviceContext* contex);
	Dust();
	Dust(const Dust& other);
	Dust(Dust&& other)noexcept;
	Dust& operator=(const Dust& other);
	Dust& operator =(Dust&& other)noexcept;

	const bool falling();

	bool Render();
	int GetIndexCount()const;

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();
	void IntializeBuffer();

	void ShutdownBuffers();

	void EmitParticles(const float time);
	void UpdateParticles(const float time);
	void KillParticles();

	bool UpdateBuffers();

	void RenderBuffers();

	bool UpdateBuffers(ID3D11DeviceContext*);

private:
	float gravity = -9.87;
	float airResistance = 0.5;
	 ID3D11Device* device{};
	 ID3D11DeviceContext* context{};
	float m_particleDeviationX{}, m_particleDeviationY
{}, m_particleDeviationZ{};
	float m_particleVelocity{} , m_particleVelocityVariation
{};
	float m_particleSize{}, m_particlesPerSecond
{};
	int m_maxParticles {};
	int m_currentParticleCount {};
	float m_accumulatedTime {};
	ParticleType* m_particleList {};
	int m_vertexCount{}, m_indexCount
{};
	DustVertex* m_vertex{};
	ID3D11Buffer* m_vertexBuffer{} , * m_indexBuffer
{};
	ModelManager* m_model{} ;
};

