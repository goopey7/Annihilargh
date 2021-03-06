﻿#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>


#include "../Graphics.h"
#include "../Bindable/Bindable.h"

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void Draw(Graphics &gfx) const noexcept;
	void AddBindable(std::unique_ptr<Bindable> bindable) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept;
	virtual ~Drawable() = default;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Tick(float deltaTime) noexcept = 0;
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept = 0;
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> bindables;
};
