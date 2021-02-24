#include "Cube.h"



#include "../Bindable/ConstantBuffer.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"
#include "../Bindable/IndexBuffer.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationConstantBuffer.h"

Cube::Cube(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist,
           std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist,
           std::uniform_real_distribution<float>& rdist)
		: offsetX(rdist(rng)),dPitch(ddist(rng)),dYaw(ddist(rng)),dRoll(ddist(rng)),
		theta(adist(rng)),phi(adist(rng)),chi(adist(rng)),
		dTheta(odist(rng)),dPhi(odist(rng)),dChi(odist(rng))
{
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
	};
	const std::vector<Vertex> vertices =
	{
		{1.f,1.f,1.f},
		{-1.f,1.f,1.f},
		{1.f,-1.f,1.f},
		{-1.f,-1.f,1.f},
		{1.f,1.f,-1.f},
		{-1.f,1.f,-1.f},
		{1.f,-1.f,-1.f},
		{-1.f,-1.f,-1.f},
	};
	AddBindable(std::make_unique<VertexBuffer>(gfx,vertices));

	auto pVertexShader = std::make_unique<VertexShader>(gfx,L"VertexShader.cso");
	auto pVertexShaderBlob = pVertexShader->GetBlob();
	AddBindable(std::move(pVertexShader));

	AddBindable(std::make_unique<PixelShader>(gfx,L"PixelShader.cso"));

	// index buffer for indexed drawing
	const std::vector<unsigned short> indices =
	{
		7,5,6, 5,4,6,
        6,4,2, 4,0,2,
        5,1,4, 4,1,0,
        3,2,0, 3,0,1,
        7,3,5, 5,3,1,
        7,6,3, 6,2,3,
    };
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx,indices));

	struct PixelCB
	{
		struct
		{
			float r,g,b,a;
		}faceColours[6];
	};

	const PixelCB pcb=
	{
{
			{1.f,0.f,1.f},
			{1.f,0.f,0.f},
			{0.f,1.f,0.f},
			{0.f,0.f,1.f},
			{1.f,1.f,0.f},
			{0.f,1.f,1.f},
		 }
	};
	AddBindable(std::make_unique<PixelConstantBuffer<PixelCB>>(gfx,pcb));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
				D3D11_INPUT_PER_VERTEX_DATA,0}
		};
	AddBindable(std::make_unique<InputLayout>(gfx,ied,pVertexShaderBlob));
	AddBindable(std::make_unique<Topology>(gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBindable(std::make_unique<TransformationConstantBuffer>(gfx,*this));
}

void Cube::Tick(float deltaTime) noexcept
{
	pitch+=dPitch*deltaTime;
	yaw+=dYaw*deltaTime;
	roll+=dRoll*deltaTime;
	theta+=dTheta*deltaTime;
	phi+=dPhi*deltaTime;
	chi+=dChi*deltaTime;
}

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch,yaw,roll)*
		DirectX::XMMatrixTranslation(offsetX,0.f,0.f)*
		DirectX::XMMatrixRotationRollPitchYaw(theta,phi,chi)*
		DirectX::XMMatrixTranslation(0.f,0.f,20.f);
}
