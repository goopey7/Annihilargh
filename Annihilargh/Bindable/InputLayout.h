#pragma once
#include <vector>

#include "Bindable.h"
#include "../Graphics.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics &gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC> &layout,
		ID3DBlob* pVertexBlob);
	void Bind(Graphics &gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};
