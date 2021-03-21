#pragma once
#include <DirectXMath.h>
#include <vector>

struct ARGBColour
{
	unsigned char a, r, g, b;
};

// describes how the dumb char buffer in VertexBuffer is structured
class VertexLayout
{
public:
	enum ElementType
	{
		Location2D,
		Location3D,
		TextureCoord2D,
		Normal,
		Float3Colour,
		Float4Colour,
		ARGBColour
	};

	class Element
	{
	public:
		Element(ElementType type, size_t offset) : type(type), offset(offset)
		{
		}

		size_t GetOffsetOfNextElement() const noexcept
		{
			return offset + Size();
		}

		// constexpr so it won't take any runtime resources
		static constexpr size_t SizeOf(ElementType type) noexcept
		{
			switch(type)
			{
			case Location2D:
				return sizeof(DirectX::XMFLOAT2);
			case Location3D:
				return sizeof(DirectX::XMFLOAT3);
			case TextureCoord2D:
				return sizeof(DirectX::XMFLOAT2);
			case Normal:
				return sizeof(DirectX::XMFLOAT3);
			case Float3Colour:
				return sizeof(DirectX::XMFLOAT3);
			case Float4Colour:
				return sizeof(DirectX::XMFLOAT4);
			case ARGBColour:
				return sizeof(::ARGBColour);
			}
			assert("Unknown Element Type" && false);
			return 0u;
		}

		size_t Size() const noexcept
		{
			return SizeOf(type);
		}

		ElementType GetType() const noexcept
		{
			return type;
		}

		size_t GetOffset() const noexcept
		{
			return offset;
		}

	private:
		ElementType type;
		size_t offset; // number of bytes away from the beginning of the Vertex struct
	};

	template <ElementType T>
	// access element that matches the type. Returns constant reference to it.
	const Element& Get() const noexcept
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

	const Element& Get(size_t index) const noexcept
	{
		return elements[index];
	}

	template <ElementType T>
	VertexLayout& Append() noexcept
	{
		elements.emplace_back(T, Size());
		return *this;
	}

	size_t Size() const noexcept
	{
		// if vector is empty returns 0. Otherwise returns the size of the struct in bytes
		// offset after last element is the size of the structure
		return elements.empty() ? 0u : elements.back().GetOffsetOfNextElement();
	}

	size_t NumElements() const noexcept
	{
		return elements.size();
	}

private:
	std::vector<Element> elements;
};

// represents a vertex within the vertex buffer, and can retrieve attributes
class Vertex
{
	friend class VertexBuffer;
public:
	template <VertexLayout::ElementType T>
	auto& Retrieve() noexcept
	{
		namespace dx = DirectX;
		const auto &element = layout.Get<T>();
		auto pAttribute = pData + element.GetOffset();

		// I want to use constexpr here so this is determined at compile time
		// so no switch statement I'm afraid
		if constexpr(T == VertexLayout::Location2D)
		{
			return *reinterpret_cast<dx::XMFLOAT2*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::Location3D)
		{
			return *reinterpret_cast<dx::XMFLOAT3*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::TextureCoord2D)
		{
			return *reinterpret_cast<dx::XMFLOAT2*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::Normal)
		{
			return *reinterpret_cast<dx::XMFLOAT3*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::Float3Colour)
		{
			return *reinterpret_cast<dx::XMFLOAT3*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::Float4Colour)
		{
			return *reinterpret_cast<dx::XMFLOAT4*>(pAttribute);
		}
		else if constexpr(T == VertexLayout::ARGBColour)
		{
			return *reinterpret_cast<ARGBColour*>(pAttribute);
		}
		else
		{
			assert("Unknown Element Type" && false);
			return *pAttribute;
		}
	}

	template <typename T>
	// the double && allows us to forward a parameter without loosing any information or modifiers
	// we then use std::forward to forward it as is. This is using universal references and perfect forwarding
	void SetAttributeByIndex(size_t i, T &&val) noexcept
	{
		namespace dx = DirectX;
		const auto &element = layout.Get(i);
		auto pAttribute = pData + element.GetOffset();
		switch(element.GetType())
		{
		case VertexLayout::Location2D:
			SetAttribute<dx::XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Location3D:
			SetAttribute<dx::XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::TextureCoord2D:
			SetAttribute<dx::XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<dx::XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Colour:
			SetAttribute<dx::XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Colour:
			SetAttribute<dx::XMFLOAT4>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::ARGBColour:
			SetAttribute<ARGBColour>(pAttribute, std::forward<T>(val));
			break;
		default:
			assert("Unknown Element Type" && false);
		}
	}

protected:
	Vertex(char* pData, const VertexLayout &layout) noexcept : pData(pData), layout(layout)
	{
		assert(pData != nullptr);
	}
private:
	// We don't know how many parameters we're going to get
	// so the ... represents a 'parameter pack' meaning it can represent any number of parameters
	// you can't iterate on this pack, so we have to do recursion.
	// A quick explanation is that this function will peel off the first parameter from the pack, and process it.
	// Then we do recursion and keep doing that until we have processed every parameter.
	template <typename First, typename ...Rest>
	void SetAttributeByIndex(size_t i, First &&first, Rest &&...rest) noexcept
	{
		SetAttributeByIndex(i, std::forward<First>(first)); // evaluates to the above public SetAttributeByIndex()
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...); // recurse, peels back first, recurses again.
		// once rest contains only one parameter, this call will resolve to the above SetAttributeByIndex()
		// so that's how the recursion will end.
	}

	// this function checks to see if the type we are attempting to cast to will actually work.
	// it's called in the public SetAttributByIndex()
	template <typename Dest, typename Src>
	void SetAttribute(char* pAttribute, Src &&val) noexcept
	{
		if constexpr(std::is_assignable<Dest, Src>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
			assert("Fail to cast attribute type" && false);
	}

private:
	char* pData = nullptr;
	const VertexLayout &layout;
};

// Read-Only Vertex
class ConstVertex
{
public:
	ConstVertex(const Vertex &v) noexcept : vertex(v)
	{
	}

	template <VertexLayout::ElementType T>
	const auto& Retrieve() const noexcept
	{
		return const_cast<Vertex&>(vertex).Retrieve<T>();
	}

private:
	Vertex vertex;
};

// represents the data
class VertexBuffer
{
public:
	VertexBuffer(VertexLayout layout) noexcept : layout(std::move(layout))
	{
	}

	const VertexLayout& GetLayout() const noexcept
	{
		return layout;
	}

	size_t Size() const noexcept // Gets the number of vertices, not the size in bytes
	{
		return buffer.size() / layout.Size();
	}

	Vertex Front() noexcept
	{
		assert(buffer.size()>=0u);
		return Vertex{buffer.data(), layout};
	}

	Vertex Back() noexcept
	{
		assert(buffer.size()>=0u);
		return Vertex{buffer.data() + buffer.size() - layout.Size(), layout};
	}

	Vertex operator[](size_t i) noexcept
	{
		assert(i < Size());
		return Vertex{buffer.data() + layout.Size() * i, layout};
	}

	ConstVertex Back() const noexcept
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex Front() const noexcept
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex operator[](size_t i) const noexcept
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}

	// takes in a parameter pack and processes them. Refer to the private field of Vertex for more info
	template <typename ...Params>
	void EmplaceBack(Params &&... params) noexcept
	{
		// the number of parameters should match the amount of elements in the layout
		assert(
			sizeof...(params) == layout.NumElements() && "Number of params doesn't match number of elements in layout");
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}

private:
	std::vector<char> buffer; // unstructured buffer of bytes
	VertexLayout layout; // layout describing the structure
};
