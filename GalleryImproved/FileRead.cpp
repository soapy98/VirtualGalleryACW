#include "FileRead.h"
#include <fstream>
#include "DirectXMath.h"
FileRead::FileRead():camera1_(),camera2_(),spot_light_(),static_light_()
{
	
}
void FileRead::ReadFile(const std::string& filename)
{
	std::ifstream fin(filename);
	XMFLOAT4 x;
	
	std::string attribute, at2;
	float parameter;
	char delim, delim2;
	fin >> attribute;
	if (attribute == "Camera1:")
	{
		fin >> at2;
		if (at2 == "Eye")
		{
			fin >> parameter >> delim >> delim2;
			//fin >> parameter;
			camera1_[0].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[0].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[0].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[0].m128_f32[3] = parameter;
		}
		fin >> at2;
		if (at2 == "At")
		{
			fin >> parameter >> delim >> delim2;
			camera1_[1].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[1].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[1].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[1].m128_f32[3] = parameter;
		}
		fin >> at2;
		if (at2 == "Up")
		{
			fin >> parameter >> delim >> delim2;
			camera1_[2].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[2].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[2].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera1_[2].m128_f32[3] = parameter;
		}
	}
	fin >> attribute;
	if (attribute == "Camera2:")
	{
		fin >> at2;
		if (at2 == "Eye")
		{
			fin >> parameter >> delim >> delim2;
			//fin >> parameter;
			camera2_[0].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[0].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[0].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[0].m128_f32[3] = parameter;
		}
		fin >> at2;
		if (at2 == "At")
		{
			fin >> parameter >> delim >> delim2;
			camera2_[1].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[1].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[1].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[1].m128_f32[3] = parameter;
		}
		fin >> at2;
		if (at2 == "Up")
		{
			fin >> parameter >> delim >> delim2;
			camera2_[2].m128_f32[0] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[2].m128_f32[1] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[2].m128_f32[2] = parameter;
			fin >> parameter >> delim >> delim2;
			camera2_[2].m128_f32[3] = parameter;
		}

	}
	fin >> attribute;
	if (attribute == "SpotLights:")
	{
		for (auto& y: spot_light_)
		{
			fin >> x.x >> x.y >> x.z >> x.w;
			y = x;
		}
	}
	fin >> attribute;
	if (attribute == "SingleLight:")
	{
		fin >> static_light_.x >> static_light_.y >> static_light_.z >> static_light_.z;
	}
	fin.close();
}

const XMVECTOR* FileRead::GetCamera1()const 
{
	return camera1_;
}

const XMVECTOR* FileRead::GetCamera2()const 
{
	return camera2_;
}



XMFLOAT4* FileRead::GetSpotLights()const
{
	return const_cast<XMFLOAT4*>(spot_light_);
}

XMFLOAT4 FileRead::GetStaticLight()const 
{
	return static_light_;
}

FileRead::FileRead(const FileRead& other)=default;

FileRead& FileRead::operator=(const FileRead& other) = default;

FileRead& FileRead::operator=(FileRead&& other) noexcept = default;

FileRead::FileRead(FileRead&& other) noexcept = default;




