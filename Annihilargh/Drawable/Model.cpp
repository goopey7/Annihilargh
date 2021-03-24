#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


#include "../VertexSystem.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformationCB.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"

#include "../imgui/imgui.h"

#include <unordered_map>


#include "../Bindable/Sampler.h"
#include "../Bindable/Texture.h"

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

Node::Node(int id, const std::string &name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX &_transform) noexcept
	: meshes(std::move(meshPtrs)), name(name), id(id)
{
	DirectX::XMStoreFloat4x4(&transform, _transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::RenderTree(Node* &pSelectedNode) const noexcept
{
	// determine selectedId
	const int selectedId = pSelectedNode == nullptr ? -1 : pSelectedNode->GetId();
	// flags for current node
	const auto flags = ImGuiTreeNodeFlags_OpenOnArrow |
		// if there is no selected int in the optional, value_or will evaluate it to -1
		(GetId() == selectedId ? ImGuiTreeNodeFlags_Selected : 0) |
		(children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	// if tree node is expanded, recursively render the children
	// yeah had to do the cast to void ptr here with intptr_t. You have to for ImGui.
	const auto bIsExpanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), flags, name.c_str());
	if(ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	if(bIsExpanded)
	{
		for(const auto &pChild : children)
		{
			pChild->RenderTree(pSelectedNode);
		}
		ImGui::TreePop(); // we are done displaying tree node contents
	}
}

void Node::Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	// recurses down the tree
	const auto currentTransform =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		accumulatedTransform;
	for(const auto pMesh : meshes)
	{
		pMesh->Draw(gfx, currentTransform);
	}
	for(const auto &pChild : children)
	{
		pChild->Draw(gfx, currentTransform);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetId() const noexcept
{
	return id;
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
	assert(pChild);
	children.push_back(std::move(pChild));
}

// yep we're declaring a full on class in a cpp file
// this is fine because it's private and only the Model needs to know about it.
class ModelWindow
{
public:
	void Show(const char* windowName, const Node &root) noexcept
	{
		// if windowName is null call it Model
		windowName = windowName ? windowName : "Model";
		int nodeIndexTracker = 0; // used to provide unique Node indices
		if(ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true); // true for border
			root.RenderTree(pSelectedNode);

			ImGui::NextColumn();
			if(pSelectedNode != nullptr)
			{
				auto &modelTransform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Location");
				ImGui::SliderFloat("X", &modelTransform.x, -20.f, 20.f);
				ImGui::SliderFloat("Y", &modelTransform.y, -20.f, 20.f);
				ImGui::SliderFloat("Z", &modelTransform.z, -20.f, 20.f);

				ImGui::Text("Rotation");
				ImGui::SliderAngle("Pitch", &modelTransform.pitch, -180.f, 180.f);
				ImGui::SliderAngle("Yaw", &modelTransform.yaw, -180.f, 180.f);
				ImGui::SliderAngle("Roll", &modelTransform.roll, -180.f, 180.f);

				ImGui::Text("Scale");
				ImGui::SliderFloat("Scale", &modelTransform.scale, .001f, 1.f);

				if(ImGui::Button("Reset"))
				{
					modelTransform = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f};
				}
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(pSelectedNode!=nullptr);
		const auto &transform = transforms.at(pSelectedNode->GetId());
		return
			DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
			// why does roll pitch yaw take in pitch yaw roll? lol
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}

private:
	struct TransformParams
	{
		float x = 0.f, y = 0.f, z = 0.f, pitch = 0.f, yaw = 0.f, roll = 0.f, scale = 1.f;
	};

	Node* pSelectedNode = nullptr;
	std::unordered_map<int, TransformParams> transforms;
};

Model::Model(Graphics &gfx, const std::string fileName) : pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
	                                 aiProcess_Triangulate |
	                                 aiProcess_JoinIdenticalVertices);

	// load all the meshes
	for(size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshes.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics &gfx, const aiMesh &mesh, const aiMaterial* const* pMaterials)
{
	namespace dx = DirectX;
	using dVS::VertexLayout;

	dVS::VertexBuffer vb(std::move(
		VertexLayout{}
		.Append(VertexLayout::Location3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::TextureCoord2D)
	));

	for(unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vb.EmplaceBack(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
		               *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
		               *reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		// there are 8 channels for texture coords
		// we just want channel 0
	}

	std::vector<std::unique_ptr<Bindable>> bindables;
	if(mesh.mMaterialIndex >= 0) // index will be negative if mesh doesn't have an index
	{
		using namespace std::string_literals;
		auto &material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0u, &texFileName); // fills filename str with filename

		std::string filePath = "3DAssets\\nanosuitTextures\\"s +
			texFileName.C_Str();
		bindables.push_back(std::make_unique<Texture>(gfx, Image::FromFile(filePath)));
		bindables.push_back(std::make_unique<Sampler>(gfx));
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for(unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto &face = mesh.mFaces[i];
		assert("Face has more than 3 indices" && face.mNumIndices==3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	bindables.push_back(std::make_unique<VertexBuffer>(gfx, vb));
	bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
	auto pBlob = pVertexShader->GetBlob();
	bindables.push_back(std::move(pVertexShader));

	bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
	bindables.push_back(std::make_unique<InputLayout>(gfx, vb.GetLayout().GetD3DLayout(), pBlob));

	struct MaterialCB
	{
		float specularPower = 30.f;
		float specularIntensity = .6f;
		float padding[2];
	} matCB;

	bindables.push_back(std::make_unique<PixelConstantBuffer<MaterialCB>>(gfx, matCB, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindables));
}

std::unique_ptr<Node> Model::ParseNode(int &nextId, const aiNode &node) noexcept
{
	namespace dx = DirectX;

	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)));

	// the nodes only have dumb references to other meshes, while entire model has the smart ptr
	std::vector<Mesh*> currentMeshes;
	currentMeshes.reserve(node.mNumMeshes);
	for(size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		currentMeshes.push_back(meshes[meshIdx].get());
	}

	// load children recursively
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(currentMeshes), transform);
	for(size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}

void Model::Draw(Graphics &gfx) const
{
	// apply any transformations
	if(auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	// draw to all children
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}
