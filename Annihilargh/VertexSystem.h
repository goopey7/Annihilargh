#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgiformat.h>
#include <vector>

#include "Colour.h"

namespace dVS
{
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

		// This will allow us to map type-names to a structure that contains compile-time data.
		// This provides one single place for all this data to be defined, so it'll be much harder to create bugs.
		// It also made the Retrieve function in Vertex a lot cleaner and more slick
		template <ElementType>
		struct Map;

		template <>
		struct Map<Location2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			// had to use static constexpr here so that these are done at compile-time, and I don't have to instantiate
			// the structure before accessing these values, which would have been annoying.
		};

		template <>
		struct Map<Location3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};

		template <>
		struct Map<TextureCoord2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
		};

		template <>
		struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
		};

		template <>
		struct Map<Float3Colour>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Colour";
		};

		template <>
		struct Map<Float4Colour>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Colour";
		};

		template <>
		struct Map<ARGBColour>
		{
			using SysType = ::ARGBColour;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Colour";
		};

		class Element
		{
		public:
			Element(ElementType type, size_t offset);
			size_t GetOffsetOfNextElement() const noexcept;
			// constexpr so it won't take any runtime resources
			static constexpr size_t SizeOf(ElementType type) noexcept;
			size_t Size() const noexcept;
			ElementType GetType() const noexcept;
			size_t GetOffset() const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept;
		private:
			template <ElementType T>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateInputElementDesc(size_t offset);

		private:
			ElementType type;
			size_t offset; // number of bytes away from the beginning of the Vertex struct
		};

		template <ElementType T>
		// access element that matches the type. Returns constant reference to it.
		const Element& Get() const noexcept;
		
		const Element& Get(size_t index) const noexcept;
		VertexLayout& Append(ElementType type) noexcept;
		size_t Size() const noexcept;
		size_t NumElements() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept;

	private:
		std::vector<Element> elements;
	};
	
	// represents a vertex within the vertex buffer, and can retrieve attributes
	class Vertex
	{
		friend class VertexBuffer;
	public:
		template <VertexLayout::ElementType T>
		auto& Retrieve() noexcept;

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
				SetAttribute<VertexLayout::Location2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Location3D:
				SetAttribute<VertexLayout::Location3D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::TextureCoord2D:
				SetAttribute<VertexLayout::TextureCoord2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float3Colour:
				SetAttribute<VertexLayout::Float3Colour>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float4Colour:
				SetAttribute<VertexLayout::Float4Colour>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ARGBColour:
				SetAttribute<VertexLayout::ARGBColour>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Unknown Element Type" && false);
			}
		}

	protected:
		Vertex(char* pData, const VertexLayout &layout) noexcept;

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
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...); // recurse, peels back first parameter
			// once rest contains only one parameter, this call will resolve to the above SetAttributeByIndex()
			// so that's how the recursion will end.
		}

		// this function checks to see if the type we are attempting to cast to will actually work.
		// it's called in the public SetAttributeByIndex()
		template <VertexLayout::ElementType DestLayout, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType &&val) noexcept
		{
			using DestSysType = typename VertexLayout::Map<DestLayout>::SysType;
			if constexpr(std::is_assignable<DestSysType, SrcType>::value)
			{
				*reinterpret_cast<DestSysType*>(pAttribute) = val;
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
		ConstVertex(const Vertex &v) noexcept;

		template <VertexLayout::ElementType T>
		const auto& Retrieve() const noexcept;
	private:
		Vertex vertex;
	};

	// represents the data
	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept;
		const char* GetData() const noexcept;
		const VertexLayout& GetLayout() const noexcept;
		size_t Size() const noexcept; // Gets the number of vertices, not the size in bytes
		size_t SizeBytes() const noexcept;
		Vertex Front() noexcept;
		Vertex Back() noexcept;
		Vertex operator[](size_t i) noexcept;
		ConstVertex Back() const noexcept;
		ConstVertex Front() const noexcept;
		ConstVertex operator[](size_t i) const noexcept;

		// takes in a parameter pack and processes them
		// Refer to the private field and implementations in Vertex for more info
		template <typename ...Params>
		void EmplaceBack(Params &&... params) noexcept
		{
			// the number of parameters should match the amount of elements in the layout
			assert(
                sizeof...(params) == layout.NumElements() &&
                "Number of params doesn't match number of elements in layout");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
	private:
		std::vector<char> buffer; // unstructured buffer of bytes
		VertexLayout layout; // layout describing the structure
	};
}
