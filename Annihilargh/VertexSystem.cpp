#include "VertexSystem.h"

// ******** VERTEX LAYOUT ********
dVS::VertexLayout::Element::Element(ElementType type, size_t offset)
	: type(type), offset(offset)
{
}

size_t dVS::VertexLayout::Element::GetOffsetOfNextElement() const noexcept
{
	return offset + Size();
}

size_t dVS::VertexLayout::Element::Size() const noexcept
{
	return SizeOf(type);
}

dVS::VertexLayout::ElementType dVS::VertexLayout::Element::GetType() const noexcept
{
	return type;
}

size_t dVS::VertexLayout::Element::GetOffset() const noexcept
{
	return offset;
}

D3D11_INPUT_ELEMENT_DESC dVS::VertexLayout::Element::GetDesc() const noexcept
{
	switch(type)
	{
	case Location2D:
		return GenerateInputElementDesc<Location2D>(GetOffset());
	case Location3D:
		return GenerateInputElementDesc<Location3D>(GetOffset());
	case TextureCoord2D:
		return GenerateInputElementDesc<TextureCoord2D>(GetOffset());
	case Normal:
		return GenerateInputElementDesc<Normal>(GetOffset());
	case Float3Colour:
		return GenerateInputElementDesc<Float3Colour>(GetOffset());
	case Float4Colour:
		return GenerateInputElementDesc<Float4Colour>(GetOffset());
	case ARGBColour:
		return GenerateInputElementDesc<ARGBColour>(GetOffset());
	}
	assert("Unknown Element Type" && false);
	return {
		"UNKNOWN", 0u, DXGI_FORMAT_UNKNOWN, 0u, 0u,
		D3D11_INPUT_PER_VERTEX_DATA, 0u
	};
}

template <dVS::VertexLayout::ElementType T>
const dVS::VertexLayout::Element& dVS::VertexLayout::Get() const noexcept
{
	for(auto &e : elements)
	{
		if(e.GetType() == T)
		{
			return e;
		}
	}
	assert("Could not find element type" && false);
	return elements.back();
}

const dVS::VertexLayout::Element& dVS::VertexLayout::Get(size_t index) const noexcept
{
	return elements[index];
}

dVS::VertexLayout& dVS::VertexLayout::Append(ElementType type) noexcept
{
	elements.emplace_back(type, Size());
	return *this;
}

size_t dVS::VertexLayout::Size() const noexcept
{
	// if vector is empty returns 0. Otherwise returns the size of the struct in bytes
	// offset after last element is the size of the structure
	return elements.empty() ? 0u : elements.back().GetOffsetOfNextElement();
}

size_t dVS::VertexLayout::NumElements() const noexcept
{
	return elements.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> dVS::VertexLayout::GetD3DLayout() const noexcept
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
	desc.reserve(NumElements());
	for(const auto &e : elements)
	{
		desc.push_back(e.GetDesc());
	}
	return desc;
}

constexpr size_t dVS::VertexLayout::Element::SizeOf(ElementType type) noexcept
{
	switch(type)
	{
	case Location2D:
		return sizeof(Map<Location2D>::SysType);
	case Location3D:
		return sizeof(Map<Location3D>::SysType);
	case TextureCoord2D:
		return sizeof(Map<TextureCoord2D>::SysType);
	case Normal:
		return sizeof(Map<Normal>::SysType);
	case Float3Colour:
		return sizeof(Map<Float3Colour>::SysType);
	case Float4Colour:
		return sizeof(Map<Float4Colour>::SysType);
	case ARGBColour:
		return sizeof(Map<ARGBColour>::SysType);
	}
	assert("Unknown Element Type" && false);
	return 0u;
}

template <dVS::VertexLayout::ElementType T>
constexpr D3D11_INPUT_ELEMENT_DESC dVS::VertexLayout::Element::GenerateInputElementDesc(size_t offset)
{
	return
	{
		Map<T>::semantic, 0u, Map<T>::dxgiFormat, 0u, (UINT)offset,
		D3D11_INPUT_PER_VERTEX_DATA, 0u
	};
}

// ******** VERTEX ********
template <dVS::VertexLayout::ElementType T>
auto& dVS::Vertex::Retrieve() noexcept
{
	auto pAttribute = pData + layout.Get<T>().GetOffset();
	return *reinterpret_cast<typename VertexLayout::Map<T>::SysType*>(pAttribute);
}

dVS::Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept
: pData(pData), layout(layout)
{
	assert(pData != nullptr);
}

// ******** CONST VERTEX ********
dVS::ConstVertex::ConstVertex(const Vertex &v) noexcept
: vertex(v)
{
}

template <dVS::VertexLayout::ElementType T>
const auto& dVS::ConstVertex::Retrieve() const noexcept
{
	return const_cast<Vertex&>(vertex).Retrieve<T>();
}

// ******** VERTEX BUFFER ********
dVS::VertexBuffer::VertexBuffer(VertexLayout layout) noexcept
: layout(std::move(layout))
{
}

const char* dVS::VertexBuffer::GetData() const noexcept
{
	return buffer.data();
}

const dVS::VertexLayout& dVS::VertexBuffer::GetLayout() const noexcept
{
	return layout;
}

size_t dVS::VertexBuffer::Size() const noexcept
{
	return buffer.size() / layout.Size();
}

size_t dVS::VertexBuffer::SizeBytes() const noexcept
{
	return buffer.size();
}

dVS::Vertex dVS::VertexBuffer::Front() noexcept
{
	assert(buffer.size()>=0u);
	return Vertex{buffer.data(), layout};
}

dVS::Vertex dVS::VertexBuffer::Back() noexcept
{
	assert(buffer.size()>=0u);
	return Vertex{buffer.data() + buffer.size() - layout.Size(), layout};
}

dVS::Vertex dVS::VertexBuffer::operator[](size_t i) noexcept
{
	assert(i < Size());
	return Vertex{buffer.data() + layout.Size() * i, layout};
}

dVS::ConstVertex dVS::VertexBuffer::Back() const noexcept
{
	return const_cast<VertexBuffer*>(this)->Back();
}

dVS::ConstVertex dVS::VertexBuffer::Front() const noexcept
{
	return const_cast<VertexBuffer*>(this)->Front();
}

dVS::ConstVertex dVS::VertexBuffer::operator[](size_t i) const noexcept
{
	return const_cast<VertexBuffer&>(*this)[i];
}
