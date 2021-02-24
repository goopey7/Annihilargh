#pragma once
#include "Bindable.h"
#include "../Graphics.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics &gfx, const std::wstring &path);
	void Bind(Graphics &gfx) noexcept override;
	ID3DBlob* GetBlob() const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};
