#include "TransformationConstantBuffer.h"

TransformationConstantBuffer::TransformationConstantBuffer(Graphics& gfx, const Drawable& parent)
	: vertexConstantBuffer(gfx),parent(parent)
{}

void TransformationConstantBuffer::Bind(Graphics& gfx) noexcept
{
	vertexConstantBuffer.Tick(gfx,
		DirectX::XMMatrixTranspose(parent.GetTransformXM()*gfx.GetProjection()));
	vertexConstantBuffer.Bind(gfx);
}
