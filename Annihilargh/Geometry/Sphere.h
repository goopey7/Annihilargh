#pragma once
#include <activation.h>

#include "Math.h"

#include "IndexedTriangleList.h"

namespace Geometry
{
	class Sphere
	{
	public:
		template <class V>
		static IndexedTriangleList<V> CreateTessellated(int latDiv, int longDiv)
		{
			namespace dx = DirectX;
			assert(latDiv>=3&&longDiv>=3);

			constexpr float radius = 1.f;
			const auto base = dx::XMVectorSet(0.f, 0.f, radius, 0.f);
			const float latitudeAngle = PI / latDiv;
			const float longitudeAngle = 2.f * PI / longDiv;

			std::vector<V> vertices;
			for (int lat = 1; lat < latDiv; lat++)
			{
				const auto latBase = dx::XMVector3Transform(
					base, dx::XMMatrixRotationX(latitudeAngle * lat)
				);
				for (int longitude = 0; longitude < longDiv; longitude++)
				{
					vertices.emplace_back(); // add vertex to list
					auto v = dx::XMVector3Transform(
						latBase, dx::XMMatrixRotationZ(longitudeAngle * longitude)
					);
					dx::XMStoreFloat3(&vertices.back().pos, v); // fill new vertex
				}
			}

			// add cap vertices
			const unsigned short northPole = (unsigned short)vertices.size();
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, base);
			const unsigned short southPole = (unsigned short)vertices.size();
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, dx::XMVectorNegate(base));

			std::vector<unsigned short> indices;

			// calculate indices
			for (unsigned short lat = 0; lat < latDiv - 2; lat++)
			{
				for (unsigned short longitude = 0; longitude < longDiv - 1; longitude++)
				{
					indices.push_back(lat * longDiv + longitude);
					indices.push_back((lat + 1) * longDiv + longitude);
					indices.push_back(lat * longDiv + longitude + 1);
					indices.push_back(lat * longDiv + longitude + 1);
					indices.push_back((lat + 1) * latDiv + longitude);
					indices.push_back((lat + 1) * longDiv + longitude + 1);
				}
				indices.push_back(lat * longDiv + longDiv - 1);
				indices.push_back((lat + 1) * longDiv + longDiv - 1);
				indices.push_back(lat * longDiv);
				indices.push_back(lat * longDiv);
				indices.push_back((lat + 1) * longDiv + longDiv - 1);
				indices.push_back((lat + 1) * longDiv);
			}
			// do the north/south caps
			for (unsigned short longitude = 0; longitude < longDiv - 1; longitude++)
			{
				// north cap
				indices.push_back(northPole);
				indices.push_back(longitude);
				indices.push_back(longitude + 1);
				// south cap
				indices.push_back((latDiv - 2) * longDiv + longitude + 1);
				indices.push_back((latDiv - 2) * longDiv + longitude);
				indices.push_back(southPole);
			}

			//wrap triangles
			// north pole
			indices.push_back(northPole);
			indices.push_back(longDiv - 1);
			indices.push_back(0);
			// south pole
			indices.push_back((latDiv - 2) * longDiv);
			indices.push_back((latDiv - 2) * longDiv + longDiv - 1);
			indices.push_back(southPole);

			return
			{
				std::move(vertices), std::move(indices)
			};
		}

		template <class V>
		static IndexedTriangleList<V> Create()
		{
			return CreateTessellated<V>(12, 24);
		}
	};
}
