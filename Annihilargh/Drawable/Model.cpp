#include "Model.h"

#include "../VertexSystem.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationCB.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"

Mesh::Mesh(Graphics &gfx, std::vector<std::unique_ptr<Bindable>> bindables)
{
	if(!IsStaticDataInitialised())
		AddStaticBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	for(auto &pBind : bindables)
	{
		if(auto pIndexBuffer = dynamic_cast<IndexBuffer*>(pBind.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{pIndexBuffer});
			pBind.release(); // we only want one unique pointer to be dealing with memory, so we release here.
			// so now the unique ptr being bound is the only smart ptr dealing with the index buffer
		}
		else AddBindable(std::move(pBind));
	}
	AddBindable(std::make_unique<TransformationCB>(gfx, *this));
}

void Mesh::Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

Node::Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept
: meshes(std::move(meshPtrs))
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
}
void Node::Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	// recurses down the tree
	const auto currentTransform = DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
	for(const auto pMesh : meshes)
	{
		pMesh->Draw(gfx, currentTransform);
	}
	for(const auto &pChild : children)
	{
		pChild->Draw(gfx, currentTransform);
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
	assert(pChild);
	children.push_back(std::move(pChild));
}

Model::Model(Graphics& gfx, const std::string fileName)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices);

	// load all the meshes
	for(size_t i=0;i<pScene->mNumMeshes;i++)
	{
		meshes.push_back(ParseMesh(gfx,*pScene->mMeshes[i]));
	}

	pRoot = ParseNode(*pScene->mRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
	namespace dx = DirectX;
	using dVS::VertexLayout;

	dVS::VertexBuffer vb(std::move(VertexLayout{}.Append(VertexLayout::Location3D).Append(VertexLayout::Normal)));

	for(unsigned int i = 0;i<mesh.mNumVertices; i++)
	{
		vb.EmplaceBack(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]));
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for(unsigned int i=0;i<mesh.mNumFaces;i++)
	{
		const auto &face = mesh.mFaces[i];
		assert("Face has more than 3 indices" && face.mNumIndices==3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bindable>> bindables;
	bindables.push_back(std::make_unique<VertexBuffer>(gfx,vb));
	bindables.push_back(std::make_unique<IndexBuffer>(gfx,indices));

	auto pVertexShader = std::make_unique<VertexShader>(gfx,L"PhongVS.cso");
	auto pBlob = pVertexShader->GetBlob();
	bindables.push_back(std::move(pVertexShader));

	bindables.push_back(std::make_unique<PixelShader>(gfx,L"PhongPS.cso"));
	bindables.push_back(std::make_unique<InputLayout>(gfx,vb.GetLayout().GetD3DLayout(),pBlob));

	struct MaterialCB
	{
		dx::XMFLOAT3 colour = {1.f,.3f,.3f};
		float specularIntensity = .6f;
		float specularPower = 30.f;
		float padding[3];
	} matCB;

	bindables.push_back(std::make_unique<PixelConstantBuffer<MaterialCB>>(gfx,matCB,1u));
	
	return std::make_unique<Mesh>(gfx,std::move(bindables));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node)
{
	namespace dx = DirectX;

	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)));

	// the nodes only have dumb references to other meshes, while entire model has the smart ptr
	std::vector<Mesh*> currentMeshes;
	currentMeshes.reserve(node.mNumMeshes);
	for(size_t i=0;i<node.mNumMeshes;i++)
	{
		const auto meshIdx = node.mMeshes[i];
		currentMeshes.push_back(meshes[meshIdx].get());
	}

	// load children recursively
	auto pNode = std::make_unique<Node>(std::move(currentMeshes),transform);
	for(size_t i=0;i<node.mNumChildren;i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}

void Model::Draw(Graphics& gfx, DirectX::FXMMATRIX startingTransform) const
{
	// start the recursion down the tree at the root
	pRoot->Draw(gfx,startingTransform);
}
