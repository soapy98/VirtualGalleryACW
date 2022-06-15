#pragma once
#include <DirectXMath.h>

#include <cmath>
using namespace DirectX;
class Camera
{
public:
	Camera();
	~Camera();
	const XMMATRIX& GetProjectionMatrix() const;
	const XMMATRIX& GetViewMatrix() const;
	void UpdateViewMatrix();
	void AddPitch(float in);
	void AddYaw(float in);

	void AdjustPosition(const float x, const float y,const float z);
	void SetLookAtPos(XMFLOAT3 &lookAtPos);
	void PanVerticle(float in);
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void SetPosition(const XMVECTOR& inPos);
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetPosVector() const;
	 float CompareFloat(const float A, const float B, const float eps = 0.005f) const
	{
		return std::fabs(A - B) < eps;
	}
private:
	float pitch{}, yaw{};
	XMVECTOR defaultRight = XMVectorSet(1, 0, 0, 0);
	XMVECTOR defaultForward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR Right = XMVectorSet(1, 0, 0, 0);
	XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR target = XMVectorSet(0, 1, 0, 0);
	XMVECTOR pos = XMVectorSet(0, 0, 0, 0);
	XMMATRIX projectionMatrix;
	XMFLOAT3 m_pos{};
};

