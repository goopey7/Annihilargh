#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>

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
	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX &transform) noexcept;

	void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
private:

	void AddChild(std::unique_ptr<Node> pChild) noexcept;

	std::vector<std::unique_ptr<Node>> children;
	std::vector<Mesh*> meshes;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model(Graphics &gfx, const std::string fileName);
	static std::unique_ptr<Mesh> ParseMesh(Graphics &gfx, const aiMesh &mesh);
	std::unique_ptr<Node> ParseNode(const aiNode &node);
	void Draw(Graphics &gfx) const;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshes;
};
