﻿#pragma once
#include "Bindable.h"
#include "../Image.h"

class Texture : public Bindable
{
public:
	Texture(Graphics &gfx, const class Image &img, unsigned int slot = 0);
	void Bind(Graphics &gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
private:
	unsigned int slot;
};
