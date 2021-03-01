#pragma once
#include "IndexedTriangleList.h"

namespace Geometry
{
	class Cube
	{
	public:
		template <class V>
		static IndexedTriangleList<V> Create()
		{
			constexpr float side = 1.f / 2.f;
			std::vector<DirectX::XMFLOAT3> verticesDX;
			verticesDX.emplace_back(-side, -side, -side); // 0
			verticesDX.emplace_back(side, -side, -side); // 1
			verticesDX.emplace_back(-side, side, -side); // 2
			verticesDX.emplace_back(side, side, -side); // 3
			verticesDX.emplace_back(-side, -side, side); // 4
			verticesDX.emplace_back(side, -side, side); // 5
			verticesDX.emplace_back(-side, side, side); // 6
			verticesDX.emplace_back(side, side, side); // 7

			std::vector<V> vertices(verticesDX.size());
			for (size_t i = 0; i < vertices.size(); i++)
				vertices[i].pos = verticesDX[i];

			return
			{
				std::move(vertices),
				{
					0, 2, 1,   2, 3, 1,
					1, 3, 5,   3, 7, 5,
					2, 6, 3,   3, 6, 7,
					4, 5, 7,   4, 7, 6,
					0, 4, 2,   2, 4, 6,
					0, 1, 4,   1, 5, 4
				}
			};
		}
	};
}
