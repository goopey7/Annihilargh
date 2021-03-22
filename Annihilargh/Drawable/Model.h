#pragma once
#include <optional>
#include <assimp/scene.h>


#include "DrawableBase.h"


class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics &gfx, std::vector<std::unique_ptr<Bindable>> bindables);
	void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Tick(float deltaTime) noexcept override{}
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(const std::string &name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX &_transform) noexcept;
	void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
private:
	void RenderTree(int &nodeIndexTracker,std::optional<int> &selectedIndex,Node* &pSelectedNode) const noexcept;
	void AddChild(std::unique_ptr<Node> pChild) noexcept;
	std::string name;
	std::vector<std::unique_ptr<Node>> children;
	std::vector<Mesh*> meshes;
	DirectX::XMFLOAT4X4 transform, appliedTransform;
};

class Model
{
public:
	Model(Graphics &gfx, const std::string fileName);
	~Model() noexcept;
	static std::unique_ptr<Mesh> ParseMesh(Graphics &gfx, const aiMesh &mesh);
	std::unique_ptr<Node> ParseNode(const aiNode &node) noexcept;
	void Draw(Graphics &gfx) const;
	void ShowWindow(const char* windowName=nullptr) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<class ModelWindow> pWindow;
};
