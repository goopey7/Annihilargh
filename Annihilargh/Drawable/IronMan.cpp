#include "IronMan.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Bindable/ConstantBuffer.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"
#include "../Bindable/IndexBuffer.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationCB.h"
#include "../VertexSystem.h"

IronMan::IronMan(Graphics &gfx, std::mt19937 &rng, std::uniform_real_distribution<float> &adist,
                 std::uniform_real_distribution<float> &ddist, std::uniform_real_distribution<float> &odist,
                 std::uniform_real_distribution<float> &rdist, DirectX::XMFLOAT3 material, float scale)
	: Object(rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;
	if(!IsStaticDataInitialised())
	{
		using alrg::VertexLayout;
		alrg::VertexBuffer vb(std::move(VertexLayout{}.Append(VertexLayout::Location3D)
			.Append(VertexLayout::Normal)));

		Assimp::Importer imp;
		auto pScene = imp.ReadFile("3DAssets\\IronMan.obj",
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		
		const auto pMesh = pScene->mMeshes[0]; // we're loading a single mesh here, so all good to use 0

		// we multiply each component by scale passed in from constructor
		
		for(unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vb.EmplaceBack(dx::XMFLOAT3{pMesh->mVertices[i].x*scale,
				pMesh->mVertices[i].y*scale,
				pMesh->mVertices[i].z*scale},*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i]));
		}

		// we are triangulating the model, so we know we are dealing with triangles only
		std::vector<unsigned short> indices;
		for(unsigned int i = 0; i<pMesh->mNumFaces;i++)
		{
			const auto &face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3); // gotta be a triangle
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		
		AddStaticBindable(std::make_unique<VertexBuffer>(gfx,vb));

		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pVertexShaderBlob = pVertexShader->GetBlob();
		AddStaticBindable(std::move(pVertexShader));

		AddStaticBindable(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx,indices));

		AddStaticBindable(std::make_unique<InputLayout>(gfx, vb.GetLayout().GetD3DLayout(), pVertexShaderBlob));
		AddStaticBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
		// if we aren't the first cube to be created, we still need to fill our index buffer pointer
	else SetIndexFromStatic();

	//obviously we want our instances to have varying positions and rotations, so no static here.
	AddBindable(std::make_unique<TransformationCB>(gfx, *this));

	struct PSMatCB
	{
		alignas(16)dx::XMFLOAT3 colour;
		float specularPower = 60.f;
		float specularIntensity = 6.f;
		float padding[2];
	} colourCB;
	colourCB.colour = material;
	AddBindable(std::make_unique<PixelConstantBuffer<PSMatCB>>(gfx, colourCB, 1u));
}
