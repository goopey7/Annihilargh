#pragma once
#include "Drawable.h"
#include "../Bindable/IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
public:
	// assuming every Drawable has at least one Bindable
	bool IsStaticDataInitialised() const noexcept
	{
		return !staticBindables.empty();
	}
	
	void AddStaticBindable(std::unique_ptr<Bindable> bindable) noexcept
	{
		// don't want to add index buffers here
		assert("Attempted to add an index buffer!!" && typeid(*bindable)!=typeid(IndexBuffer));
		staticBindables.push_back(std::move(bindable));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
	{
		// and we don't want to add an index buffer when we already have one
		assert("Already added index buffer" && pIndexBuffer == nullptr);
		pIndexBuffer=indexBuffer.get();
		staticBindables.push_back(std::move(indexBuffer));
	}

	// for Drawables that don't do the static construction. They still need to fill their indexbuffer pointer
	// so this function will do just that by looping through the bindables until one casts to an IndexBuffer.
	// Then we fill the indexbuffer pointer
	void SetIndexFromStatic() noexcept
	{
		assert("Already added index buffer" && pIndexBuffer == nullptr);
		for(const auto &b : staticBindables)
		{
			if(const auto pI = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer=pI;
				return;
			}
		}
		assert("No index buffer found in static bindables" && pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept override
	{
		return staticBindables;
	}
	static std::vector<std::unique_ptr<Bindable>> staticBindables;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBindables;
