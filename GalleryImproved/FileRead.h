#pragma once
#include <string>
#include "DirectXMath.h"
using namespace DirectX;
class FileRead
{
private:
	XMVECTOR camera1_[3];
	XMVECTOR camera2_[3];
	XMFLOAT4 spot_light_[4];
	XMFLOAT4 static_light_;
public:
	FileRead();
	void ReadFile(const std::string& filename);
	const XMVECTOR* GetCamera1()const;
	const XMVECTOR* GetCamera2()const;
	XMFLOAT4* GetSpotLights()const;
	XMFLOAT4 GetStaticLight() const;
	FileRead(const FileRead& other);
	FileRead& operator=(const FileRead& other);
	FileRead& operator=(FileRead&& other) noexcept;
	FileRead(FileRead&& other) noexcept; // Move Constructor
	~FileRead()=default;
};
