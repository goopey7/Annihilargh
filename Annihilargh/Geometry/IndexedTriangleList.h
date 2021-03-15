#pragma once

#include <cassert>
#include <DirectXMath.h>
#include <vector>

// can deal with any type of vertex class/struct. Just has to have a public pos member. 
template <class T>
class IndexedTriangleList
{
public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;

	IndexedTriangleList() = default;

	IndexedTriangleList(std::vector<T> verticesIn, std::vector<unsigned short> indicesIn)
		: vertices(std::move(verticesIn)), indices(std::move(indicesIn))
	{
		assert(vertices.size()>2);
		assert(indices.size()%3==0);
	}

	void Transform(DirectX::FXMMATRIX transformMatrix)
	{
		for(auto &v : vertices)
		{
			// load the pos as an XMVECTOR so we can do math on it
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			// once math is done we want to put it back into a Float3 for storage
			DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, transformMatrix));
		}
	}

	// if the mesh's faces are not flat, but they are independent, this will calculate and set the normals
	void SetNormalsIndependentFlat() noexcept
	{
		using namespace DirectX;
		assert(indices.size() % 3 == 0 && indices.size() > 0);
		for(size_t i = 0; i < indices.size(); i += 3)
		{
			auto &v0 = vertices[indices[i]];
			auto &v1 = vertices[indices[i + 1]];
			auto &v2 = vertices[indices[i + 2]];
			const auto p0 = XMLoadFloat3(&v0.pos);
			const auto p1 = XMLoadFloat3(&v1.pos);
			const auto p2 = XMLoadFloat3(&v2.pos);

			const auto norm = XMVector3Normalize(XMVector3Cross(p1 - p0, p2 - p0));

			XMStoreFloat3(&v0.norm, norm);
			XMStoreFloat3(&v1.norm, norm);
			XMStoreFloat3(&v2.norm, norm);
		}
	}
};
