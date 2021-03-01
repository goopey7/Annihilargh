#pragma once

#include <cassert>
#include <DirectXMath.h>
#include <vector>

// can deal with any type of vertex class/struct. Just has to have a public pos member. 
template<class T>
class IndexedTriangleList
{
public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
	
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verticesIn,std::vector<unsigned short> indicesIn)
	: vertices(std::move(verticesIn)),indices(std::move(indicesIn))
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
			DirectX::XMStoreFloat3(&v.pos,DirectX::XMVector3Transform(pos,transformMatrix));
		}
	}
};
