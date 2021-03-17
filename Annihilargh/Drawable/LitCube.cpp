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

LitCube::LitCube(Graphics &gfx, std::mt19937 &rng, std::uniform_real_distribution<float> &adist,
                 std::uniform_real_distribution<float> &ddist,
                 std::uniform_real_distribution<float> &odist,
                 std::uniform_real_distribution<float> &rdist,
                 DirectX::XMFLOAT3 material)
	                 : Object(rng,adist,ddist,odist,rdist)
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
		AddStaticBindable(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pVertexShaderBlob = pVertexShader->GetBlob();
		AddStaticBindable(std::move(pVertexShader));

		AddStaticBindable(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{
				"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};
		AddStaticBindable(std::make_unique<InputLayout>(gfx, ied, pVertexShaderBlob));
		AddStaticBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
		// if we aren't the first cube to be created, we still need to fill our index buffer pointer
	else SetIndexFromStatic();

	//obviously we want our instances to have varying positions and rotations, so no static here.
	AddBindable(std::make_unique<TransformationCB>(gfx, *this));

	struct PSMatCB
	{
		alignas(16)dx::XMFLOAT3 colour;
		float specularPower = 60.f;
		float specularIntensity = 6.f;
		float padding[2];
	} colourCB;
	colourCB.colour = material;
	AddBindable(std::make_unique<PixelConstantBuffer<PSMatCB>>(gfx, colourCB, 1u));
}
