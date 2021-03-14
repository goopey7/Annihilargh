#include "TransformationConstantBuffer.h"

TransformationConstantBuffer::TransformationConstantBuffer(Graphics& gfx, const Drawable& parent)
	:parent(parent)
{
	if(!pVertexConstantBuffer)
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
}

void TransformationConstantBuffer::Bind(Graphics& gfx) noexcept
{
	pVertexConstantBuffer->Tick(gfx,
		DirectX::XMMatrixTranspose(parent.GetTransformXM()*gfx.GetCamera()*gfx.GetProjection()));
	pVertexConstantBuffer->Bind(gfx);
}
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformationConstantBuffer::pVertexConstantBuffer;
