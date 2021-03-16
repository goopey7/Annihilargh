#include <memory>

#include "TransformationCB.h"

TransformationCB::TransformationCB(Graphics &gfx, const Drawable &parent, UINT slot)
	: parent(parent)
{
	if(!pVertexConstantBuffer)
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
}

void TransformationCB::Bind(Graphics &gfx) noexcept
{
	const auto model = parent.GetTransformXM();
	const Transforms transforms =
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(model * gfx.GetCamera() * gfx.GetProjection())
	};
	pVertexConstantBuffer->Tick(gfx, transforms);
	pVertexConstantBuffer->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformationCB::Transforms>> TransformationCB::pVertexConstantBuffer;
