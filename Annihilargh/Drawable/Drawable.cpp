#include "Drawable.h"

#include "../Bindable/IndexBuffer.h"

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for(auto &b : bindables)
		b->Bind(gfx);
	for(auto &b : GetStaticBindables())
		b->Bind(gfx);
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBindable(std::unique_ptr<Bindable> bindable) noexcept
{
	// don't want to add index buffers here
	assert("Attempted to add an index buffer!!" && typeid(*bindable)!=typeid(IndexBuffer));
	bindables.push_back(std::move(bindable));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
{
	// and we don't want to add an index buffer when we already have one
	assert("Already added index buffer" && pIndexBuffer == nullptr);
	pIndexBuffer=indexBuffer.get();
	bindables.push_back(std::move(indexBuffer));
}