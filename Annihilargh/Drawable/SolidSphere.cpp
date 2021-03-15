#include "SolidSphere.h"



#include "../Bindable/ConstantBuffer.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationCB.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"
#include "../Geometry/Sphere.h"

SolidSphere::SolidSphere(Graphics &gfx, float radius)
{
	namespace dx = DirectX;
	if(!IsStaticDataInitialised())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};
		auto model = Geometry::Sphere::Create<Vertex>();
		model.Transform(dx::XMMatrixScaling(radius,radius,radius));
		AddStaticBindable(std::make_unique<VertexBuffer>(gfx,model.vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx,model.indices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx,L"SolidVS.cso");
		auto pVSBlob = pVertexShader->GetBlob();
		AddStaticBindable(std::move(pVertexShader));

		AddStaticBindable(std::make_unique<PixelShader>(gfx,L"SolidPS.cso"));

		struct PSColourConst
		{
			dx::XMFLOAT3 colour = {1.f,1.f,1.f};
			float padding; // this struct should add up to 16 bytes, so we add a float.
		} colourConst;
		AddStaticBindable(std::make_unique<PixelConstantBuffer<PSColourConst>>(gfx,colourConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
					D3D11_INPUT_PER_VERTEX_DATA,0}
			};
		AddStaticBindable(std::make_unique<InputLayout>(gfx,ied,pVSBlob));
		
		AddStaticBindable(std::make_unique<Topology>(gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else SetIndexFromStatic();
	AddBindable(std::make_unique<TransformationCB>(gfx,*this));
}

void SolidSphere::Tick(float deltaTime) noexcept {}

void SolidSphere::SetLocation(DirectX::XMFLOAT3 _pos) noexcept
{
	pos = _pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x,pos.y,pos.z);
}
