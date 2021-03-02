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

			// calculate indices
			std::vector<unsigned short> indices;
			// nice lambda function for doing just that

			const auto calcIndex = [latDiv,longDiv](unsigned short lat, unsigned short longitude)
			{
				return lat * longDiv + longitude;
			};
			
			for (unsigned short lat = 0; lat < latDiv - 2; lat++)
			{
				for (unsigned short longitude = 0; longitude < longDiv - 1; longitude++)
				{
					indices.push_back(calcIndex(lat, longitude));
					indices.push_back(calcIndex(lat + 1, longitude));
					indices.push_back(calcIndex(lat, longitude + 1));
					indices.push_back(calcIndex(lat, longitude + 1));
					indices.push_back(calcIndex(lat + 1, longitude));
					indices.push_back(calcIndex(lat + 1, longitude + 1));
				}
				indices.push_back(calcIndex(lat, longDiv - 1));
				indices.push_back(calcIndex(lat + 1, longDiv - 1));
				indices.push_back(calcIndex(lat, 0));
				indices.push_back(calcIndex(lat, 0));
				indices.push_back(calcIndex(lat + 1, longDiv - 1));
				indices.push_back(calcIndex(lat + 1, 0));
			}

			// do the north/south caps
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				// north cap
				indices.push_back(northPole);
				indices.push_back(calcIndex(0, iLong));
				indices.push_back(calcIndex(0, iLong + 1));
				// south cap
				indices.push_back(calcIndex(latDiv - 2, iLong + 1));
				indices.push_back(calcIndex(latDiv - 2, iLong));
				indices.push_back(southPole);
			}
			// wrap triangles
			// north pole
			indices.push_back(northPole);
			indices.push_back(calcIndex(0, longDiv - 1));
			indices.push_back(calcIndex(0, 0));
			// south pole
			indices.push_back(calcIndex(latDiv - 2, 0));
			indices.push_back(calcIndex(latDiv - 2, longDiv - 1));
			indices.push_back(southPole);

			return {std::move(vertices), std::move(indices)};
		}

		template <class V>
		static IndexedTriangleList<V> Create()
		{
			return CreateTessellated<V>(10, 40);
		}
	};
}
