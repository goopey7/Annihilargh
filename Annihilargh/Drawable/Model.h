#pragma once
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
public:
	Node(const std::string &name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX &transform) noexcept;
	void RenderTree() const noexcept;
	void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
private:

	void AddChild(std::unique_ptr<Node> pChild) noexcept;

	std::string name;
	std::vector<std::unique_ptr<Node>> children;
	std::vector<Mesh*> meshes;
	DirectX::XMFLOAT4X4 transform;
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
