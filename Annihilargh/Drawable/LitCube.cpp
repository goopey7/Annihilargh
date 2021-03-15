#include "LitCube.h"
#include "../Geometry/Cube.h"

#include <DirectXMath.h>



#include "../Bindable/ConstantBuffer.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"
#include "../Bindable/IndexBuffer.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationCB.h"

LitCube::LitCube(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist,
           std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist,
           std::uniform_real_distribution<float>& rdist)
		: offsetX(rdist(rng)),dPitch(ddist(rng)),dYaw(ddist(rng)),dRoll(ddist(rng)),
		theta(adist(rng)),phi(adist(rng)),chi(adist(rng)),
		dTheta(odist(rng)),dPhi(odist(rng)),dChi(odist(rng))
{
	namespace dx = DirectX;
	if(!IsStaticDataInitialised())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 norm;
		};

		auto model = Geometry::Cube::CreateIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		AddStaticBindable(std::make_unique<VertexBuffer>(gfx,model.vertices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx,L"PhongVS.cso");
		auto pVertexShaderBlob = pVertexShader->GetBlob();
		AddStaticBindable(std::move(pVertexShader));

		AddStaticBindable(std::make_unique<PixelShader>(gfx,L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx,model.indices));

		struct PSLightCB
		{
			dx::XMVECTOR pos;
		};
		AddStaticBindable(std::make_unique<PixelConstantBuffer<PSLightCB>>(gfx));
		
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
                D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,
    D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		AddStaticBindable(std::make_unique<InputLayout>(gfx,ied,pVertexShaderBlob));
		AddStaticBindable(std::make_unique<Topology>(gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	// if we aren't the first cube to be created, we still need to fill our index buffer pointer
	else SetIndexFromStatic();
	
	//obviously we want our instances to have varying positions and rotations, so no static here.
	AddBindable(std::make_unique<TransformationCB>(gfx,*this));
}

void LitCube::Tick(float deltaTime) noexcept
{
	pitch+=dPitch*deltaTime;
	yaw+=dYaw*deltaTime;
	roll+=dRoll*deltaTime;
	theta+=dTheta*deltaTime;
	phi+=dPhi*deltaTime;
	chi+=dChi*deltaTime;
}

DirectX::XMMATRIX LitCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch,yaw,roll)*
		DirectX::XMMatrixTranslation(offsetX,0.f,0.f)*
		DirectX::XMMatrixRotationRollPitchYaw(theta,phi,chi);
}
