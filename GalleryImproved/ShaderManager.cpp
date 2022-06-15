#include "ShaderManager.h"

ShaderManager::ShaderManager(CComPtr<ID3D11Device>& dev, CComPtr<ID3D11DeviceContext>& cont, HWND const hwnd,
                             const int textureWidth, const int textureHeight) : device_(dev), context_(cont),m_intialize_state_(),
	m_shader(), m_texture(), resource_(nullptr), sample_state(nullptr),
	_depth_stencil_view(nullptr), _render_target_view(nullptr)
{
	m_shader = new Shaders(dev, cont);
	if (m_shader->GetIntializeState())
	{
		m_intialize_state_ = false;
		MessageBox(hwnd, (LPCWSTR)"Could not initialize the Shader", (LPCWSTR)"Error", MB_OK);
		return;
	}
	m_shader->SetShader();
	m_texture = new Texture(dev, cont);
	/*if (m_texture->getInitializeState())
	{
		intilaizeState = false;
		MessageBox(hwnd, (LPCWSTR)"Could not initialize the Shader", (LPCWSTR)"Error", MB_OK);
		return;
	}*/
	m_texture->LoadAllTextures();
}

ShaderManager::ShaderManager(const ShaderManager& other) = default;

ShaderManager::ShaderManager(ShaderManager && other) noexcept = default;
ShaderManager& ShaderManager::operator=(const ShaderManager & other) = default;

ShaderManager& ShaderManager::operator=(ShaderManager && other) noexcept = default;
void ShaderManager::ShaderPass1Choice(const int choice) const
{
	switch (choice)
	{
	case 0:
		GouroudShaderSet();
		break;
	case 1:
		PhongShaderSet();
		break;
	case 2:
		BumpMappingShaderSet();
		break;
	case 3:
		ToonShaderSet1();
		break;
	case 4:
		GlowingShader1Set();
		break;
	case 5:
		TransparencyShaderSet();
	default:
		break;
	}
}
void ShaderManager::ShaderPass2Choice(const int choice) const
{
	switch (choice)
	{
	case 3:
		ToonShaderSet2();
		break;
	case 4:
		GlowingShader2Set();
		break;
	default:
		break;
	}
}

void ShaderManager::PhongShaderSet() const
{
	m_shader->SetPixel("PhongPS");
	m_shader->SetVertex("VS");
}

void ShaderManager::GouroudShaderSet() const
{
	m_shader->SetVertex("GouroudVS");
	m_shader->SetPixel("GouroudPS");

}

void ShaderManager::ToonShaderSet1() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("ToonPS");

}
void ShaderManager::ToonShaderSet2() const
{
	m_shader->SetVertex("OutlineVS");
	m_shader->SetPixel("OutlinePS");
	//a = gen.GetSamplerState("sample_clamp");
	//_pImmediateContext->PSSetSamplers(0, 1, &a);
}
void ShaderManager::DustShaderSet() const
{
	m_shader->SetVertex("DustVS");
	m_shader->SetPixel("DustPS");
	m_texture->SetTexture(5, "FireBaseCol");
}


void ShaderManager::Dust2ShaderSet() const
{
	m_shader->SetVertex("ParticleVertexShader");
	m_shader->SetPixel("ParticlePixelShader");
	m_texture->SetTexture(0, "FireBaseCol");
}
void ShaderManager::BasicShader() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("BasicPS");
	m_texture->SetTexture(0, "FireBaseCol");
}
void ShaderManager::HDR() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("HDRPS");
}

ShaderManager::~ShaderManager()
{
	delete m_shader;
	delete m_texture;

}
void ShaderManager::ScreenTest() const
{
	m_shader->SetVertex("ScreenVS");
	m_shader->SetPixel("PS");
}

void ShaderManager::GlowingShader1Set() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("PS");
}
void ShaderManager::GlowingShader2Set() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("FirePS1");
	m_texture->SetTexture(0, "FireBaseCol");

}
void ShaderManager::UnbindShaderResource(const int ShaderPass, ID3D11RenderTargetView*const RTV) const
{
	ID3D11ShaderResourceView* const pSRV[1] = { NULL }; //Create null pointer
	const FLOAT clear[4] = { 0.0,0.0,0.0,0.0 };
	context_->PSSetShaderResources(ShaderPass, 1, pSRV); //Unbind resource so it can be used in the next loop
	context_->ClearRenderTargetView(RTV, clear); // Clear the rendered texture
}

void ShaderManager::BumpMappingShaderSet() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("BumpMap");
	m_texture->SetTexture(0, "ScaleColor");
	m_texture->SetTexture(1, "ScaleAmbient");
}
void ShaderManager::TransparencyShaderSet() const
{
	m_shader->SetVertex("VS");
	m_shader->SetPixel("transPS");
}
bool ShaderManager::IsSecondPass(const int pass)
{
	switch (pass)
	{
	case 0:
		return false;
		break;
	case 1:
		return false;
		break;
	case 2:
		return false;
		break;
	case 3:
		return true;
		break;
	case 4:
		return true;
		break;
	case 5:
		return false;
		break;

	default:
		return false;
		break;
	}
}


//
//void ShaderManager::EnvironmentMapShaderSet()
//{
//}

void ShaderManager::ShadowShaderPass1() const
{
	m_shader->SetVertex("ShadowVS1");
	m_shader->SetPixel("ShadowPS1");

}

void ShaderManager::ShadowShaderPass2() const
{
	m_shader->SetVertex("ShadowVS2");
	m_shader->SetPixel("ShadowPS2");
	m_texture->SetTexture(0, "ScaleColor");
}
